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

#include <iostream>

uint64_t resever_bits_in_bytes(uint64_t v) {
  v = ((v >> 1) & 0x5555555555555555ull) | ((v & 0x5555555555555555ull) << 1);
  v = ((v >> 2) & 0x3333333333333333ull) | ((v & 0x3333333333333333ull) << 2);
  v = ((v >> 4) & 0x0F0F0F0F0F0F0F0Full) | ((v & 0x0F0F0F0F0F0F0F0Full) << 4);
  return v;
}

lczero::V3TrainingData get_v3_training_data(
    lczero::GameResult game_result, const lczero::PositionHistory& history,
    lczero::Move played_move) {

  lczero::V3TrainingData result;
  memset(&result, 0, sizeof(lczero::V3TrainingData));

  // Set version.
  result.version = 3;

  // Populate probabilities, for supervised learning simply assign 1.0 to the
  // move that was effectively played
  result.probabilities[played_move.as_nn_index()];

  // Populate planes.
  lczero::InputPlanes planes = EncodePositionForNN(history, 8);
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
  } else if (game_result == lczero::GameResult::BLACK_WON) {
    result.result = position.IsBlackToMove() ? 1 : -1;
  } else {
    result.result = 0;
  }

  return result;
}

void write_one_game_training_data(pgn_t* pgn, int game_id) {
  std::vector<lczero::V3TrainingData> training_data;
  lczero::ChessBoard starting_board;
  starting_board.SetFromFen(lczero::ChessBoard::kStartingFen, nullptr, nullptr);
  lczero::PositionHistory position_history;
  position_history.Reset(starting_board, 0, 0);
  board_t board[1];
  board_start(board);
  char str[256];
  lczero::TrainingDataWriter writer(game_id);

  lczero::GameResult game_result;
  if (my_string_equal(pgn->result, "1-0")) {
    game_result = lczero::GameResult::WHITE_WON;
  } else if (my_string_equal(pgn->result, "0-1")) {
    game_result = lczero::GameResult::BLACK_WON;
  } else {
    game_result = lczero::GameResult::DRAW;
  }

  while (pgn_next_move(pgn, str, 256)) {
    // Extract move from pgn
    int move = move_from_san(str, board);
    if (move == MoveNone || !move_is_legal(move, board)) {
      std::cout << "illegal move \"" << str << "\" at line " << pgn->move_line
                << ", column " << pgn->move_column;
      break;
    }

    // Convert move to lc0 format
    if (!move_to_can(move, board, str, 256)) ASSERT(false);
    lczero::Move lc0_move(str, colour_is_black(board->turn));

    // Generate training data
    lczero::V3TrainingData chunk =
        get_v3_training_data(game_result, position_history, lc0_move);

    // Execute move
    position_history.Append(lc0_move);
    move_do(board, move);

    writer.WriteChunk(chunk);
  }

  writer.Finalize();
}

int main(int argc, char* argv[]) {
  polyglot_init();
  int game_id = 0;
  while (*++argv) {
    printf("%s\n", *argv);
    pgn_t pgn[1];
    pgn_open(pgn, *argv);
    while (pgn_next_game(pgn)) {
      write_one_game_training_data(pgn, game_id++);
    }
    pgn_close(pgn);
  }
}