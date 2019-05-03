#include "chess/position.h"
#include "move.h"
#include "move_do.h"
#include "move_gen.h"
#include "move_legal.h"
#include "neural/encoder.h"
#include "neural/network.h"
#include "neural/writer.h"
#include "pgn.h"
#include "polyglot_lib.h"
#include "san.h"
#include "square.h"
#include "util.h"

#include <cmath>
#include <cstring>
#include <fstream>
#include <iostream>
#include <regex>
#include <sstream>
#include <thread>

size_t max_games_per_directory = 10000;
int64_t max_games_to_convert = 10000000;

struct Options {
  bool verbose = false;
  bool lichess_mode = false;
};

inline bool file_exists(const std::string& name) {
  std::ifstream f(name.c_str());
  return f.good();
}

uint64_t resever_bits_in_bytes(uint64_t v) {
  v = ((v >> 1) & 0x5555555555555555ull) | ((v & 0x5555555555555555ull) << 1);
  v = ((v >> 2) & 0x3333333333333333ull) | ((v & 0x3333333333333333ull) << 2);
  v = ((v >> 4) & 0x0F0F0F0F0F0F0F0Full) | ((v & 0x0F0F0F0F0F0F0F0Full) << 4);
  return v;
}

bool extract_lichess_comment_score(const char* comment, float& Q) {
  std::string s(comment);
  static std::regex rgx("\\[%eval (-?\\d+\\.\\d+)\\]");
  static std::regex rgx2("\\[%eval #(-?\\d+)\\]");
  std::smatch matches;
  if (std::regex_search(s, matches, rgx)) {
    Q = std::stof(matches[1].str());
    return true;
  } else if (std::regex_search(s, matches, rgx2)) {
    Q = matches[1].str().at(0) == '-' ? -128.0f : 128.0f;
    return true;
  }
  return false;
}

float convert_sf_score_to_win_probability(float score) {
  return 2 / (1 + exp(-0.4 * score)) - 1;
}

lczero::Move poly_move_to_lc0_move(move_t move, board_t* board) {
  lczero::BoardSquare from(square_rank(move_from(move)),
                           square_file(move_from(move)));
  lczero::BoardSquare to(square_rank(move_to(move)),
                         square_file(move_to(move)));
  lczero::Move m(from, to);

  if (move_is_promote(move)) {
    lczero::Move::Promotion lookup[5] = {
        lczero::Move::Promotion::None,   lczero::Move::Promotion::Knight,
        lczero::Move::Promotion::Bishop, lczero::Move::Promotion::Rook,
        lczero::Move::Promotion::Queen,
    };
    auto prom = lookup[move >> 12];
    m.SetPromotion(prom);
  } else if (move_is_castle(move, board)) {
    bool is_short_castle =
        square_file(move_from(move)) < square_file(move_to(move));
    int file_to = is_short_castle ? 6 : 2;
    m.SetTo(lczero::BoardSquare(square_rank(move_to(move)), file_to));
    m.SetCastling();
  }

  if (colour_is_black(board->turn)) {
    m.Mirror();
  }

  return m;
}

lczero::V4TrainingData get_v4_training_data(
    lczero::GameResult game_result, const lczero::PositionHistory& history,
    lczero::Move played_move, lczero::MoveList legal_moves, float Q) {
  lczero::V4TrainingData result;

  // Set version.
  result.version = 4;

  // Illegal moves will have "-1" probability
  std::memset(result.probabilities, -1.0f, sizeof(result.probabilities));

  // Populate legal moves with probability "0"
  for (lczero::Move move : legal_moves) {
    result.probabilities[move.as_nn_index()] = 0;
  }

  // Assign "1" (100%) to the move that was actually played
  result.probabilities[played_move.as_nn_index()] = 1.0f;

  // Populate planes.
  lczero::InputPlanes planes =
      EncodePositionForNN(history, 8, lczero::FillEmptyHistory::FEN_ONLY);
  int plane_idx = 0;
  for (auto& plane : result.planes) {
    plane = resever_bits_in_bytes(planes[plane_idx++].mask);
  }

  const auto& position = history.Last();
  // Populate castlings.
  result.castling_us_ooo =
      position.CanCastle(lczero::Position::WE_CAN_OOO) ? 1 : 0;
  result.castling_us_oo =
      position.CanCastle(lczero::Position::WE_CAN_OO) ? 1 : 0;
  result.castling_them_ooo =
      position.CanCastle(lczero::Position::THEY_CAN_OOO) ? 1 : 0;
  result.castling_them_oo =
      position.CanCastle(lczero::Position::THEY_CAN_OO) ? 1 : 0;

  // Other params.
  result.side_to_move = position.IsBlackToMove() ? 1 : 0;
  result.move_count = 0;
  result.rule50_count = position.GetNoCaptureNoPawnPly();

  // Game result.
  if (game_result == lczero::GameResult::WHITE_WON) {
    result.result = position.IsBlackToMove() ? -1 : 1;
    result.root_d = result.best_d = 0.0f;
  } else if (game_result == lczero::GameResult::BLACK_WON) {
    result.result = position.IsBlackToMove() ? 1 : -1;
    result.root_d = result.best_d = 0.0f;
  } else {
    result.result = 0;
    result.root_d = result.best_d = 1.0f;
  }

  // Q for Q+Z training
  result.root_q = result.best_q = position.IsBlackToMove() ? -Q : Q;

  return result;
}

struct PGNMoveInfo {
  char move[PGN_STRING_SIZE];
  char comment[PGN_STRING_SIZE];
  char nag[PGN_STRING_SIZE];

  PGNMoveInfo(char* in_move, char* in_comment, char* in_nag) {
    strncpy(move, in_move, PGN_STRING_SIZE);
    strncpy(comment, in_comment, PGN_STRING_SIZE);
    strncpy(nag, in_nag, PGN_STRING_SIZE);
  }
};

struct PGNGame {
  char result[PGN_STRING_SIZE];
  char fen[PGN_STRING_SIZE];
  std::vector<PGNMoveInfo> moves;
};

PGNGame get_pgn_game(pgn_t* pgn) {
  PGNGame pgn_game;
  strncpy(pgn_game.result, pgn->result, PGN_STRING_SIZE);
  strncpy(pgn_game.fen, pgn->fen, PGN_STRING_SIZE);

  char str[256];
  while (pgn_next_move(pgn, str, 256)) {
    pgn_game.moves.emplace_back(str, pgn->last_read_comment,
                                pgn->last_read_nag);
  }

  return pgn_game;
}

std::vector<lczero::V4TrainingData> get_game_chunks(PGNGame pgn,
                                                    Options options) {
  std::vector<lczero::V4TrainingData> chunks(256);
  lczero::ChessBoard starting_board;
  std::string starting_fen =
      std::strlen(pgn.fen) > 0 ? pgn.fen : lczero::ChessBoard::kStartposFen;

  {
    std::istringstream fen_str(starting_fen);
    std::string board;
    std::string who_to_move;
    std::string castlings;
    std::string en_passant;
    fen_str >> board >> who_to_move >> castlings >> en_passant;
    if (fen_str.eof()) {
      starting_fen.append(" 0 0");
    }
  }

  if (options.verbose) {
    std::cout << "Started new game, starting FEN: \'" << starting_fen << "\'"
              << std::endl;
  }

  starting_board.SetFromFen(starting_fen, nullptr, nullptr);

  lczero::PositionHistory position_history;
  position_history.Reset(starting_board, 0, 0);
  board_t board[1];
  board_from_fen(board, starting_fen.c_str());

  lczero::GameResult game_result;
  if (options.verbose) {
    std::cout << "Game result: " << pgn.result << std::endl;
  }
  if (my_string_equal(pgn.result, "1-0")) {
    game_result = lczero::GameResult::WHITE_WON;
  } else if (my_string_equal(pgn.result, "0-1")) {
    game_result = lczero::GameResult::BLACK_WON;
  } else {
    game_result = lczero::GameResult::DRAW;
  }

  char str[256];
  for (auto pgn_move : pgn.moves) {
    // Extract move from pgn
    int move = move_from_san(pgn_move.move, board);
    if (move == MoveNone || !move_is_legal(move, board)) {
      std::cout << "illegal move \"" << pgn_move.move << std::endl;
      break;
    }

    if (options.verbose) {
      move_to_san(move, board, str, 256);
      std::cout << "Read move: " << str << std::endl;
      if (pgn_move.comment[0]) {
        std::cout << str << " pgn comment: " << pgn_move.comment << std::endl;
      }
    }

    bool bad_move = false;
    if (pgn_move.nag[0]) {
      // If the move is bad or dubious, skip it.
      // See https://en.wikipedia.org/wiki/Numeric_Annotation_Glyphs for PGN
      // NAGs
      if (pgn_move.nag[0] == '2' || pgn_move.nag[0] == '4' ||
          pgn_move.nag[0] == '5' || pgn_move.nag[0] == '6') {
        bad_move = true;
      }
    }

    // Extract SF scores and convert to win probability
    float Q = 0.0f;
    if (pgn_move.comment[0]) {
      float lichess_score;
      if (move_is_mate(move, board)) {
        lichess_score = position_history.Last().IsBlackToMove() ? -128.0f : 128.0f;
      } else {
        bool success = extract_lichess_comment_score(pgn_move.comment,
                                                     lichess_score);
        if (!success) {
          break;  // Comment contained no "%eval"
        }
      }
      Q = convert_sf_score_to_win_probability(lichess_score);
    } else if (options.lichess_mode) {
      // This game has no comments, skip it.
      break;
    }

    // Convert move to lc0 format
    lczero::Move lc0_move = poly_move_to_lc0_move(move, board);

    bool found = false;
    auto legal_moves = position_history.Last().GetBoard().GenerateLegalMoves();
    for (auto legal : legal_moves) {
      if (legal == lc0_move && legal.castling() == lc0_move.castling()) {
        found = true;
        break;
      }
    }
    if (!found) {
      std::cout << "Move not found: " << pgn_move.move << " "
                << square_file(move_to(move)) << std::endl;
    }

    if (!bad_move) {
      // Generate training data
      lczero::V4TrainingData chunk = get_v4_training_data(
          game_result, position_history, lc0_move, legal_moves, Q);
      chunks.push_back(chunk);
    }

    // Execute move
    position_history.Append(lc0_move);
    move_do(board, move);
  }

  if (options.verbose) {
    std::cout << "Game end." << std::endl;
  }

  return chunks;
}

void convert_games(std::string pgn_file_name, Options options) {
  int game_id = -1;
  pgn_t pgn[1];
  pgn_open(pgn, pgn_file_name.c_str());
  while (pgn_next_game(pgn) && game_id < max_games_to_convert) {
    game_id++;
    auto game = get_pgn_game(pgn);
    auto chunks = get_game_chunks(game, options);
    if (chunks.size() > 0) {
      lczero::TrainingDataWriter writer = lczero::TrainingDataWriter(
          game_id,
          "supervised-" + std::to_string(game_id / max_games_per_directory));
      for (auto chunk : chunks) {
        writer.WriteChunk(chunk);
      }
      writer.Finalize();
    }
  }
  pgn_close(pgn);
}

int main(int argc, char* argv[]) {
  lczero::InitializeMagicBitboards();
  polyglot_init();
  Options options;
  for (size_t idx = 0; idx < argc; ++idx) {
    if (0 == static_cast<std::string>("-v").compare(argv[idx])) {
      std::cout << "Verbose mode ON" << std::endl;
      options.verbose = true;
    } else if (0 ==
               static_cast<std::string>("-lichess-mode").compare(argv[idx])) {
      std::cout << "Lichess mode ON" << std::endl;
      options.lichess_mode = true;
    } else if (0 ==
               static_cast<std::string>("-games-per-dir").compare(argv[idx])) {
      max_games_per_directory = std::atoi(argv[idx + 1]);
      std::cout << "Max games per directory set to: " << max_games_per_directory
                << std::endl;
    } else if (0 == static_cast<std::string>("-max-games-to-convert")
                        .compare(argv[idx])) {
      max_games_to_convert = std::atoi(argv[idx + 1]);
      std::cout << "Max games to convert set to: " << max_games_to_convert
                << std::endl;
    }
  }

  for (size_t idx = 1; idx < argc; ++idx) {
    if (!file_exists(argv[idx])) continue;
    if (options.verbose) {
      std::cout << "Opening \'" << argv[idx] << "\'" << std::endl;
    }

    convert_games(argv[idx], options);
  }
}
