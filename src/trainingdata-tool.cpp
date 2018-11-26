#include "chess/position.h"
#include "neural/encoder.h"
#include "neural/network.h"
#include "neural/writer.h"

#include <PGNGameCollection.h>

#include <cstring>
#include <fstream>
#include <iostream>

uint64_t resever_bits_in_bytes(uint64_t v) {
  v = ((v >> 1) & 0x5555555555555555ull) | ((v & 0x5555555555555555ull) << 1);
  v = ((v >> 2) & 0x3333333333333333ull) | ((v & 0x3333333333333333ull) << 2);
  v = ((v >> 4) & 0x0F0F0F0F0F0F0F0Full) | ((v & 0x0F0F0F0F0F0F0F0Full) << 4);
  return v;
}

lczero::Move ply_to_lc0_move(pgn::Ply& ply, const lczero::ChessBoard& board,
                             bool mirror) {
  if (ply.isShortCastle() || ply.isLongCastle()) {
    lczero::Move m;
    int file_to = ply.isShortCastle() ? 6 : 2;
    unsigned int rowIndex = 0;
    m.SetFrom(lczero::BoardSquare(rowIndex, 4));
    m.SetTo(lczero::BoardSquare(rowIndex, file_to));
    m.SetCastling();
    return m;
  } else {
    for (auto legal_move : board.GenerateLegalMoves()) {
      const bool knight_move = board.our_knights().get(legal_move.from());
      const bool bishop_move = board.bishops().get(legal_move.from());
      const bool rook_move = board.rooks().get(legal_move.from());
      const bool queen_move = board.queens().get(legal_move.from());
      const bool king_move = board.our_king().get(legal_move.from());
      const bool pawn_move = board.pawns().get(legal_move.from());

      if (mirror) {
        legal_move.Mirror();
      }

      if (legal_move.to().row() == ply.toSquare().rowIndex() &&
          legal_move.to().col() == ply.toSquare().colIndex()) {
        auto piece = ply.piece();
        if (piece == pgn::Piece::Knight() && !knight_move ||
            piece == pgn::Piece::Bishop() && !bishop_move ||
            piece == pgn::Piece::Rook() && !rook_move ||
            piece == pgn::Piece::Queen() && !queen_move ||
            piece == pgn::Piece::King() && !king_move ||
            piece == pgn::Piece::Pawn() && !pawn_move) {
          continue;
        }

        int colIndex = ply.fromSquare().colIndex();
        if (colIndex >= 0 && legal_move.from().col() != colIndex) {
          continue;
        }

        int rowIndex = ply.fromSquare().rowIndex();
        if (rowIndex >= 0 && legal_move.from().row() != rowIndex) {
          continue;
        }

        if (ply.promotion()) {
          switch (ply.promoted().letter()) {
            case 'Q':
              legal_move.SetPromotion(lczero::Move::Promotion::Queen);
              break;
            case 'R':
              legal_move.SetPromotion(lczero::Move::Promotion::Rook);
              break;
            case 'B':
              legal_move.SetPromotion(lczero::Move::Promotion::Bishop);
              break;
            case 'N':
              legal_move.SetPromotion(lczero::Move::Promotion::Knight);
            default:
              assert(false);
          }
        }

        if (mirror) {
          legal_move.Mirror();
        }

        return legal_move;
      }
    }
  }
  assert(false);
  return {};
}

lczero::V3TrainingData get_v3_training_data(
    lczero::GameResult game_result, const lczero::PositionHistory& history,
    lczero::Move played_move) {
  lczero::V3TrainingData result;

  // Set version.
  result.version = 3;

  // Populate probabilities, for supervised learning simply assign 1.0 to the
  // move that was effectively played
  std::memset(result.probabilities, 0, sizeof(result.probabilities));
  result.probabilities[played_move.as_nn_index()] = 1.0f;

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

void write_one_game_training_data(pgn::Game& pgn, int game_id) {
  std::vector<lczero::V3TrainingData> training_data;
  lczero::ChessBoard starting_board;
  const std::string starting_fen = lczero::ChessBoard::kStartingFen;
  starting_board.SetFromFen(starting_fen, nullptr, nullptr);
  lczero::PositionHistory position_history;
  position_history.Reset(starting_board, 0, 0);
  lczero::TrainingDataWriter writer(game_id);

  lczero::GameResult game_result;
  if (pgn.result().isWhiteWin()) {
    game_result = lczero::GameResult::WHITE_WON;
  } else if (pgn.result().isBlackWin()) {
    game_result = lczero::GameResult::BLACK_WON;
  } else {
    game_result = lczero::GameResult::DRAW;
  }

  for (auto& move : pgn.moves()) {
    for (bool white : {true, false}) {
      auto& ply = white ? move.white() : move.black();
      if (!ply.valid()) {
        break;
      }
      // Convert move to lc0 format
      lczero::Move lc0_move =
          ply_to_lc0_move(ply, position_history.Last().GetBoard(), !white);

      bool found = false;
      for (auto legal :
           position_history.Last().GetBoard().GenerateLegalMoves()) {
        if (legal == lc0_move && legal.castling() == lc0_move.castling()) {
          found = true;
          break;
        }
      }

      if (!found) {
        std::cout << position_history.Last().DebugString() << std::endl;
        std::cout << "Move not found: " << ply << " " << lc0_move.as_string()
                  << " " << game_id << " " << std::endl;
        return;
      }

      // Generate training data
      lczero::V3TrainingData chunk =
          get_v3_training_data(game_result, position_history, lc0_move);

      // Execute move
      position_history.Append(lc0_move);

      writer.WriteChunk(chunk);
    }
  }

  writer.Finalize();
}

int main(int argc, char* argv[]) {
  int game_id = 0;
  while (*++argv) {
    std::cout << *argv << std::endl;
    std::ifstream pgnfile(*argv);
    pgn::GameCollection games;
    pgnfile >> games;
    for (auto& game : games) {
      write_one_game_training_data(game, game_id++);
    }
  }
}