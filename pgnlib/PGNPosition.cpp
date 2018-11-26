//
// Copyright (C) 2002 Andrea Vinzoni <vinzoni@users.sourceforge.net>
//
//     This file is part of pgnlib.
//
//     pgnlib is free software; you can redistribute it and/or modify
//     it under the terms of the GNU General Public License as published by
//     the Free Software Foundation; either version 2 of the License, or
//     (at your option) any later version.     
// 
//     pgnlib is distributed in the hope that it will be useful,
//     but WITHOUT ANY WARRANTY; without even the implied warranty of
//     MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  
// 
//     See the GNU General Public License for more details. 
// 
//     You should have received a copy of the GNU General Public License
//     along with pgnlib; if not, write to the Free Software Foundation, 
//     Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
// 

#include <sstream>
#include <vector>
#include <iostream>
#include <stdexcept>
#include <algorithm>
#include <functional>
#include <PGNPosition.h>
#include <PGNException.h>

namespace pgn
{
	struct PositionData 
	{
		std::vector<Square> squares;
		Color sideToMove;
		int moveNumber;
		int halfmoveClock;
		bool whiteKingCastlingAvailable; 
		bool whiteQueenCastlingAvailable; 
		bool blackKingCastlingAvailable; 
		bool blackQueenCastlingAvailable; 
		Square enPassantTargetSquare;
		
		class PieceMovementRule
		{
			struct PieceSquarePredicate
			{
			
				PieceSquarePredicate(const Piece &p)
				:piece(p) {}
			
				bool operator () (const Square &s)
				{
					return s.piece() == piece;
				}
				
				const Piece &piece;
			};
			
			public:
				virtual ~PieceMovementRule() {}

				virtual bool checkLegalMovement(const PositionData &position, const Square &from, const Square &to) = 0;
				bool isBlockedLine(const PositionData &position, const Square &from, const Square &to)
				{
					std::vector<Square> line;
					if (from.col() == to.col()) // line
					{
						char row_low = std::min(from.row(), to.row());
						char row_high = std::max(from.row(), to.row());
						for (char row = row_low+1; row < row_high; ++row)
						{
							  if (position.pieceAt(from.col(), row) != Piece::Null())
							  {
								  return true;
							  }
						}
						return false;
					}
					else if (from.row() == to.row()) // row  
					{
						char col_low = std::min(from.col(), to.col());
						char col_high = std::max(from.col(), to.col());
						for (char col = col_low+1; col < col_high; ++col)
						{
							  if (position.pieceAt(col, from.row()) != Piece::Null())
							  {
								  return true;
							  }
						}
						return false;
					}
					else if (from.col()-from.row() == to.col()-to.row()) // diagonal 1:  a1-b2-c3-...
					{
						char col_low = std::min(from.col(), to.col());
						char row = std::min(from.row(), to.row()) + 1;
						char col_high = std::max(from.col(), to.col());
						for (char col = col_low+1; col < col_high; ++col)
						{
							  if (position.pieceAt(col, row) != Piece::Null())
							  {
								  return true;
							  }
							  row++;
						}
						return false;
					}
					else if (from.col()+from.row() == to.col()+to.row()) // diagonal 2:  h1-g2-f3-...
					{
						char col_low = std::min(from.col(), to.col());
						char row = std::max(from.row(), to.row()) - 1;
						char col_high = std::max(from.col(), to.col());
						for (char col = col_low+1; col < col_high; ++col)
						{
							  if (position.pieceAt(col, row) != Piece::Null())
							  {
								  return true;
							  }
							  row--;
						}
						return false;
					}
					
					return false; // it was not a straight line at all

				}

				// the move is played on the position, afterwards we check if our king is en-prise
				bool isPiecePinned(const PositionData &pos, const Square &from, const Square &to)				
				{
					// we are dealing with a king move and the king cannot be pinned.
					if (from.piece() == Piece::WhiteKing() || from.piece() == Piece::BlackKing())
						return false;

					// playing the move on a new copied board
					PositionData position(pos);
					position.setPieceAt(from.piece(), to.col(), to.row());
					position.setPieceAt(Piece::Null(), from.col(), from.row());
					
					// Now looking for king in check. If found we just broke a pin or failed to cover our king.
					// Moving my king as if a queen: if I met an enemy piece I am in check. If I met a friend piece
					// or a board edge I am safe.
					// I am not looking for khnight checks as I am now only interested in broken pins. Should be
					// easy to extend to cover also this case.
					
					
					// let's find our king position
					std::vector<Square>::const_iterator king_itr;
					Piece my_king = (position.sideToMove == white ? Piece::WhiteKing() : Piece::BlackKing());
					king_itr = std::find_if(position.squares.begin(), position.squares.end(), PieceSquarePredicate(my_king));
					
					Piece his_piece;
					Piece his_queen = (position.sideToMove == white ? Piece::BlackQueen() : Piece::WhiteQueen());
					
					// straight lines
					// king goes N (orth)
					for (int i=king_itr->row()+1; i<'8'; ++i)
					{
						his_piece = (position.sideToMove == white ? Piece::BlackRook() : Piece::WhiteRook());
						
						Piece p = position.pieceAt(king_itr->col(), i);
						
						if ((p == his_piece) || (p == his_queen))
						{
							return true;
						}
						else if (p != Piece::Null())
						{
							break;
						}
					}
					// king goes S
					for (int i=king_itr->row()-1; i>'0'; --i)
					{
						his_piece = (position.sideToMove == white ? Piece::BlackRook() : Piece::WhiteRook());
						
						Piece p = position.pieceAt(king_itr->col(), i);

						if ((p == his_piece) || (p == his_queen))
						{
							return true;
						}
						else if (p != Piece::Null())
						{
							break;
						}
					}
					// king goes E
					for (int i=king_itr->col()+1; i<='h'; ++i)
					{
						his_piece = (position.sideToMove == white ? Piece::BlackRook() : Piece::WhiteRook());

						Piece p = position.pieceAt(i, king_itr->row());
						
						if ((p == his_piece) || (p == his_queen))
						{
							return true;
						}
						else if (p != Piece::Null())
						{
							break;
						}
					}
					// king goes W
					for (int i=king_itr->col()-1; i>='a'; --i)
					{
						his_piece = (position.sideToMove == white ? Piece::BlackRook() : Piece::WhiteRook());

						Piece p = position.pieceAt(i, king_itr->row());
						
						if ((p == his_piece) || (p == his_queen))
						{
							return true;
						}
						else if (p != Piece::Null())
						{
							break;
						}
					}
					// diagonal lines
					// king goes NE 
					int i,j;
					for (i=king_itr->col()+1, j=king_itr->row()+1; ; ++i, ++j)
					{
						if ((i > 'h') || (j > '8')) // hit a board edge
						{
							  break;
						}
						
						his_piece = (position.sideToMove == white ? Piece::BlackBishop() : Piece::WhiteBishop());
						
						Piece p = position.pieceAt(i, j);
						
						if ((p == his_piece) || (p == his_queen))
						{
							return true;
						}
						else if (p != Piece::Null())
						{
							break;
						}
					}

					// king goes NW
					for (i=king_itr->col()-1, j=king_itr->row()+1; ; --i, ++j)
					{
						if ((i < 'a') || (j > '8')) // hit a board edge
						{
							  break;
						}
						
						his_piece = (position.sideToMove == white ? Piece::BlackBishop() : Piece::WhiteBishop());
						
						Piece p = position.pieceAt(i, j);
						
						if ((p == his_piece) || (p == his_queen))
						{
							return true;
						}
						else if (p != Piece::Null())
						{
							break;
						}
					}
					// king goes SE
					for (i=king_itr->col()+1, j=king_itr->row()-1; ; ++i, --j)
					{
						if ((i > 'h') || (j < '1')) // hit a board edge
						{
							  break;
						}
						
						his_piece = (position.sideToMove == white ? Piece::BlackBishop() : Piece::WhiteBishop());
						
						Piece p = position.pieceAt(i, j);
						
						if ((p == his_piece) || (p == his_queen))
						{
							return true;
						}
						else if (p != Piece::Null())
						{
							break;
						}
					}

					// king goes SW
					for (i=king_itr->col()-1, j=king_itr->row()-1; ; --i, --j)
					{
						if ((i < 'a') || (j < '1')) // hit a board edge
						{
							  break;
						}
						
						his_piece = (position.sideToMove == white ? Piece::BlackBishop() : Piece::WhiteBishop());
						
						Piece p = position.pieceAt(i, j);
						
						if ((p == his_piece) || (p == his_queen))
						{
							return true;
						}
						else if (p != Piece::Null())
						{
							break;
						}
					}
					
					return false;
				}
		};
		
		class KingMovementRule : public PieceMovementRule
		{
			public:
				bool checkLegalMovement(const PositionData &position, const Square &from, const Square &to)
				{
				      // no need to check the movement because there is always one king (per side) on the board.
				      return true;
				}
		};

		class QueenMovementRule : public PieceMovementRule
		{
			public:
				bool checkLegalMovement(const PositionData &position, const Square &from, const Square &to)
				{
				      RookMovementRule rookRule;
				      BishopMovementRule bishopRule;
				      if (rookRule.checkLegalMovement(position, from, to) || 
					  bishopRule.checkLegalMovement(position, from, to)) 
						return true;
				      else
						return false;
				}
		};

		class RookMovementRule : public PieceMovementRule
		{
			public:
				bool checkLegalMovement(const PositionData &position, const Square &from, const Square &to)
				{
				      if (((from.col() == to.col()) || (from.row() == to.row()))
						&& (!isBlockedLine(position, from, to)) 
						&& (!isPiecePinned(position, from, to)))
						return true;
				      else
						return false;
				}
		};

		class BishopMovementRule : public PieceMovementRule
		{
			public:
				bool checkLegalMovement(const PositionData &position, const Square &from, const Square &to)
				{
      				      if ((from.col()-from.row() == to.col()-to.row()) 
						&& (!isBlockedLine(position, from, to))
						&& (!isPiecePinned(position, from, to)))
						return true;
				      else if ((from.col()+from.row() == to.col()+to.row()) && (!isBlockedLine(position, from, to)))
						return true;
				      else
						return false;
				}
		};

		class KnightMovementRule : public PieceMovementRule
		{
			public:
				bool checkLegalMovement(const PositionData &position, const Square &from, const Square &to)
				{
      				      if ((from.col()-to.col() == 1) && (from.row()-to.row() == 2))  
						return ! isPiecePinned(position, from, to);
      				      else if ((from.col()-to.col() == -1) && (from.row()-to.row() == 2))  
						return ! isPiecePinned(position, from, to);
      				      else if ((from.col()-to.col() == 1) && (from.row()-to.row() == -2))  
						return ! isPiecePinned(position, from, to);
      				      else if ((from.col()-to.col() == -1) && (from.row()-to.row() == -2))  
						return ! isPiecePinned(position, from, to);

				      else if ((from.row()-to.row() == 1) && (from.col()-to.col() == 2))  
						return ! isPiecePinned(position, from, to);
				      else if ((from.row()-to.row() == -1) && (from.col()-to.col() == 2))  
						return ! isPiecePinned(position, from, to);
				      else if ((from.row()-to.row() == 1) && (from.col()-to.col() == -2))  
						return ! isPiecePinned(position, from, to);
				      else if ((from.row()-to.row() == -1) && (from.col()-to.col() == -2))  
						return ! isPiecePinned(position, from, to);
				      else
				      {
						return false;				      
				      }
				}
		};

		class WhitePawnMovementRule : public PieceMovementRule
		{
			public:
				bool checkLegalMovement(const PositionData &position, const Square &from, const Square &to)
				{
				      // pawn e3 goes to e4
      				      if ((from.col() == to.col()) && (from.row()-to.row() == -1))  
						return ! isPiecePinned(position, from, to);
				      // pans e2 goes to e4
      				      else if ((from.col() == to.col()) && (from.row()-to.row() == -2))
				      {
						// e3 square must be free for a two square pawn move.
						if (position.pieceAt(from.col(), from.row()+1) == Piece::Null()) 
							return ! isPiecePinned(position, from, to);
						else
							return false;
				      }
				      else
						return false;				      
				}
		};
		class WhitePawnCaptureMovementRule : public PieceMovementRule
		{
			public:
				bool checkLegalMovement(const PositionData &position, const Square &from, const Square &to)
				{
      				      if ((from.col() == to.col() + 1) && (from.row()-to.row() == -1))  
						return ! isPiecePinned(position, from, to);
      				      else if ((from.col() == to.col() - 1) && (from.row()-to.row() == -1))  
						return ! isPiecePinned(position, from, to);
				      else
						return false;		      
				}
		};

		class BlackPawnMovementRule : public PieceMovementRule
		{
			public:
				bool checkLegalMovement(const PositionData &position, const Square &from, const Square &to)
				{
				      // e6 pawn goes to e5
      				      if ((from.col() == to.col()) && (from.row()-to.row() == 1))  
						return ! isPiecePinned(position, from, to);
				      // e7 pawn goes to e5
      				      else if ((from.col() == to.col()) && (from.row()-to.row() == 2))  
				      {
						// e6 square must be free for a two square pawn move.
						if (position.pieceAt(from.col(), from.row()-1) == Piece::Null()) 
							return ! isPiecePinned(position, from, to);
						else
							return false;
				      }
				      else
						return false;				      
				}
		};

		class BlackPawnCaptureMovementRule : public PieceMovementRule
		{
			public:
				bool checkLegalMovement(const PositionData &position, const Square &from, const Square &to)
				{
      				      if ((from.col() == to.col() + 1) && (from.row()-to.row() == 1))  
						return ! isPiecePinned(position, from, to);
      				      else if ((from.col() == to.col() - 1) && (from.row()-to.row() == 1))  
						return ! isPiecePinned(position, from, to);
 				      else
						return false;		      
				}
		};

		class FromSquareForPlyPredicate
		{
			public:
				FromSquareForPlyPredicate(const PositionData &p, const Ply &ply, PieceMovementRule *rule)
				:position(p),ply(ply),rule(rule),sideToMove(p.sideToMove) {}
			
				// a move Rae5 has been played: is the rook coming from the 's' square? 
				// It is if, in the current position:
				// - s has a rook on it
				// - s is on the 'a' line
				// - rook from 's' to e5 is a valid chess move.
				// - the path from s to e5 is clean
				bool operator () (const Square &s)
				{
					// work-around for colorless ply pieces.
					Piece piece = Piece(ply.piece().letter(), sideToMove);

					bool rightPiece = (s.piece() == piece); // 's' has a rook on it?
					if (!rightPiece) return false;

					bool matchingFromContentInPly = checkFromConstraints_(s, ply.fromSquare()); // is 's' on the 'a' line
					if (!matchingFromContentInPly) return false;

					bool validMove = rule->checkLegalMovement(position, s, ply.toSquare()); // is rook from 's' to 'e5' a chess move?
					if (!validMove) return false;

					return true;
				}
			
			private:
				const PositionData &position;
				Ply ply;
				PieceMovementRule *rule;
				Color sideToMove;
				
				// if ply is Nbd2 we must choose a knight on the 'b' column.
				bool checkFromConstraints_(Square candidate, Square ply_from)
				{
					if (ply_from.null())
					{
						return true;
					}
					              
					return (candidate.col() == ply_from.col()) || (candidate.row() == ply_from.row());
				}
		};

		PositionData()
		:sideToMove(white), moveNumber(0),halfmoveClock(0),
		 whiteKingCastlingAvailable(true), whiteQueenCastlingAvailable(true),
		 blackKingCastlingAvailable(true), blackQueenCastlingAvailable(true),
		 enPassantTargetSquare(Square())
		{}
		
		PositionData(const PositionData &other);
		PositionData& operator = (const PositionData &other);
		
		void fillFromSquare(pgn::Ply &ply);
		bool isEnPassant(const pgn::Ply &ply) const;
		bool isDoublePawnMove(const pgn::Ply &ply) const;
		bool hasPieceOnSide(const pgn::Square &square, const pgn::Piece &piece) const;
		Piece pieceAt(char col, char row) const;
		void setPieceAt(const pgn::Piece &piece, char col, char row);
	};

	PositionData::PositionData(const PositionData &other)
	{
		squares = other.squares;
		sideToMove = other.sideToMove;
		moveNumber = other.moveNumber;
		halfmoveClock = other.halfmoveClock;
		whiteKingCastlingAvailable = other.whiteKingCastlingAvailable; 
		whiteQueenCastlingAvailable = other.whiteQueenCastlingAvailable; 
		blackKingCastlingAvailable = other.blackKingCastlingAvailable; 
		blackQueenCastlingAvailable = other.blackQueenCastlingAvailable; 
		enPassantTargetSquare = other.enPassantTargetSquare;
	}
	
	PositionData& PositionData::operator = (const PositionData &other)
	{
	  	if (&other == this)
		return(*this);

		squares = other.squares;
		sideToMove = other.sideToMove;
		moveNumber = other.moveNumber;
		halfmoveClock = other.halfmoveClock;
		whiteKingCastlingAvailable = other.whiteKingCastlingAvailable; 
		whiteQueenCastlingAvailable = other.whiteQueenCastlingAvailable; 
		blackKingCastlingAvailable = other.blackKingCastlingAvailable; 
		blackQueenCastlingAvailable = other.blackQueenCastlingAvailable; 
		enPassantTargetSquare = other.enPassantTargetSquare;
		
		return *this;
	}
	
	void PositionData::fillFromSquare(pgn::Ply &ply)
	{
		Square from = ply.fromSquare();
		Square to = ply.toSquare();
		Piece piece = ply.piece();
		  
		std::vector<Square> squaresForPiece;
		std::vector<Square>::iterator from_itr;
		PieceMovementRule *rule;
		switch (piece.letter())
		{
			case 'P':
				if (sideToMove == white)
				{
					if (ply.isCapture())
						rule = new WhitePawnCaptureMovementRule();
					else
						rule = new WhitePawnMovementRule();
				}
				else
				{
					if (ply.isCapture())
						rule = new BlackPawnCaptureMovementRule();
					else
						rule = new BlackPawnMovementRule();
				}
				break;
			case 'N':
				rule = new KnightMovementRule();
				break;
			case 'B':
				rule = new BishopMovementRule();
				break;
			case 'R':
				rule = new RookMovementRule();
				break;
			case 'Q':
				rule = new QueenMovementRule();
				break;
			case 'K':
				rule = new KingMovementRule();
				break;
			default:
				std::stringstream ss;
				ss << "Invalid piece letter ." << piece.letter() << ". in ply " << ply;
				throw std::runtime_error(ss.str());
		}
			
		from_itr = std::find_if(squares.begin(), squares.end(), FromSquareForPlyPredicate(*this, ply, rule));
		delete rule;

		// No piece on the board of the same type of the ply's piece: wrong ply.
		if (from_itr == squares.end())
		{
			std::stringstream ss;
			ss << "No piece found on the board for the ply " << ply;
			throw std::runtime_error(ss.str());
		}
			
		from = Square (from_itr->col(), from_itr->row(), piece);
		ply = pgn::Ply(ply.str());
		ply.setFromSquare(from);
		ply.setPiece(piece);
	}

	bool PositionData::isEnPassant(const pgn::Ply &ply) const
	{
		// en-passant is a capture ...
		if (!ply.isCapture())
			return false;
		// ... of a pawn ...
		if (ply.piece() != Piece::Pawn() && ply.piece() != Piece::WhitePawn() && ply.piece() != Piece::BlackPawn())
			return false;
		// ... towards an empty square!
		if (pieceAt(ply.toSquare().col(), ply.toSquare().row()) != Piece::Null())
			return false;

		return true;
	}

	bool PositionData::isDoublePawnMove(const pgn::Ply &ply) const
	{
		if ((ply.piece() != Piece::WhitePawn()) && (ply.piece() != Piece::BlackPawn()))
			return false;

		if (abs(ply.toSquare().row() - ply.fromSquare().row()) == 2)
			return true;

		return false;
	}

	bool PositionData::hasPieceOnSide(const pgn::Square &square, const pgn::Piece &piece) const
	{
		if (square.col()+1 <= 'h')
			if (pieceAt(square.col()+1, square.row()) == piece)
				return true;

		if (square.col()-1 >= 'a')
			if (pieceAt(square.col()-1, square.row()) == piece)
				return true;

		return false;
	}

	Piece PositionData::pieceAt(char col, char row) const
	{
		int idx = (row-'1')*8 + (col-'a');
		return squares[idx].piece();
	}
	
	void PositionData::setPieceAt(const pgn::Piece &piece, char col, char row)
	{
		int idx = (row-'1')*8 + (col-'a');
		squares[idx] = pgn::Square(col, row, piece);
	}

	class Fen
	{
		public:
	  
			Fen(const std::string &str)
			{
				std::istringstream iss(str);
				iss >> board_ >> color_ >> castling_
				    >> en_passant_ >> halfmove_clock_ >> move_number_;
			}
			
			Fen(const PositionData &position)
			{
				color_ = (position.sideToMove == black ? "b" : "w");
				castling_ = castlingEvaluation(position); 
				en_passant_ = "-"; // unsupported
				halfmove_clock_ = position.halfmoveClock;
				move_number_ = position.moveNumber;
				board_ = "";
				for (int i=7; i>=0; --i)
				{
					int empty_square_count = 0;
				  	if (i<7) board_ += '/';
				
					for (int j=0; j<8; ++j)
					{
						Square s = position.squares[i*8 + j];
						Piece p = s.piece();
						
						if (p == Piece::Null())
						{
							empty_square_count ++;
							if (j == 7) board_ += '0' + empty_square_count;
							continue;
						}
						else if (empty_square_count > 0)
						{
							board_ += '0' + empty_square_count;
							empty_square_count = 0;
						}
						
						if (p == Piece::WhitePawn()) board_ += "P";
						else if (p == Piece::WhiteKnight()) board_ += "N";
						else if (p == Piece::WhiteBishop()) board_ += "B";
						else if (p == Piece::WhiteRook()) board_ += "R";
						else if (p == Piece::WhiteQueen()) board_ += "Q";
						else if (p == Piece::WhiteKing()) board_ += "K";
						else if (p == Piece::BlackPawn()) board_ += "p";
						else if (p == Piece::BlackKnight()) board_ += "n";
						else if (p == Piece::BlackBishop()) board_ += "b";
						else if (p == Piece::BlackRook()) board_ += "r";
						else if (p == Piece::BlackQueen()) board_ += "q";
						else if (p == Piece::BlackKing()) board_ += "k";
						else 
						{
						    std::stringstream ss;
						    ss << "invalid piece '" << p.letter() << "-" << p.color() << "' in position.";
						    throw std::runtime_error(ss.str());
						}
					}
				}
			}
			
			std::string str() const
			{
				std::stringstream ss;
				ss << board_ << " " << color_ << " " << castling_ 
				    << " " << en_passant_ << " " << halfmove_clock_ 
				    << " " << move_number_;
				return ss.str();
			}

			void setPosition(PositionData &position) const
			{
				position.sideToMove = (color_ == "b" ? pgn::black : pgn::white);
				position.moveNumber = move_number_;
				position.halfmoveClock = halfmove_clock_;
				position.enPassantTargetSquare = en_passant_ == "-" ? Square() : Square(en_passant_[0], en_passant_[1]);
				position.whiteKingCastlingAvailable = (castling_.find("K") != std::string::npos);
				position.whiteQueenCastlingAvailable = (castling_.find("Q") != std::string::npos);
				position.blackKingCastlingAvailable = (castling_.find("k") != std::string::npos);
				position.blackQueenCastlingAvailable = (castling_.find("q") != std::string::npos);
				position.squares = std::vector<pgn::Square>(64);
				char col = 'a';
				char row = '8';
				for (unsigned int i=0; i<board_.size(); ++i)
				{
					switch(board_[i])
					{
						case 'P':
							updatePosition(position, col, row, Piece::WhitePawn());
							break;
						case 'N':
							updatePosition(position, col, row, Piece::WhiteKnight());
							break;
						case 'B':
							updatePosition(position, col, row, Piece::WhiteBishop());
							break;
						case 'R':
							updatePosition(position, col, row, Piece::WhiteRook());
							break;
						case 'Q':
							updatePosition(position, col, row, Piece::WhiteQueen());
							break;
						case 'K':
							updatePosition(position, col, row, Piece::WhiteKing());
							break;
						case 'p':
							updatePosition(position, col, row, Piece::BlackPawn());
							break;
						case 'n':
							updatePosition(position, col, row, Piece::BlackKnight());
							break;
						case 'b':
							updatePosition(position, col, row, Piece::BlackBishop());
							break;
						case 'r':
							updatePosition(position, col, row, Piece::BlackRook());
							break;
						case 'q':
							updatePosition(position, col, row, Piece::BlackQueen());
							break;
						case 'k':
							updatePosition(position, col, row, Piece::BlackKing());
							break;
						case '1':
						case '2':
						case '3':
						case '4':
						case '5':
						case '6':
						case '7':
						case '8':
							for (int j=0; j<board_[i]-'0'; ++j) {
								updatePosition(position, col, row, Piece::Null());
							}
							break;
						case '/':
							// nop
							break;
						default:
							throw pgn::invalid_fen_string(str());
						  
					}
				}			  
			}
			
		private:
		  
			std::string board_;
			std::string color_;
			std::string castling_;
			std::string en_passant_;
			int halfmove_clock_;
			int move_number_;
			
			void updatePosition(PositionData &position, char &col, char &row, const Piece &piece) const
			{
			      position.setPieceAt(piece, col, row);

			      // update col, row to point the next square (a1 --> b1)
			      col += 1;
			      if (col == 'i')
			      {
					row -= 1;
					col = 'a';
			      }
			}
			
			std::string castlingEvaluation(const PositionData &position)
			{
				std::string castling = "";
				if (position.squares[4].piece() == Piece::WhiteKing())
				{
					if (position.squares[7].piece() == Piece::WhiteRook())
						castling += "K";
					if (position.squares[0].piece() == Piece::WhiteRook())
						castling += "Q";
				}

				if (position.squares[60].piece() == Piece::BlackKing())
				{
					if (position.squares[63].piece() == Piece::BlackRook())
						castling += "k";
					if (position.squares[56].piece() == Piece::BlackRook())
						castling += "q";
				}
				
				if (castling == "") castling = "-";
				return castling;
			}
 	};
};

// By default we get the position at the start of the game
pgn::Position::Position() 
{
	hdata = new pgn::PositionData;
	reset();
}

pgn::Position::Position(const std::string &fenstring) 
{
	hdata = new pgn::PositionData;
	
	Fen fen(fenstring);
	fen.setPosition(*hdata);
}

pgn::Position::Position(const pgn::Position& src) 
{
	hdata = new pgn::PositionData;
	hdata->squares = src.hdata->squares;
	hdata->sideToMove = src.hdata->sideToMove;
	hdata->moveNumber = src.hdata->moveNumber;
	hdata->halfmoveClock = src.hdata->halfmoveClock;
}

pgn::Position::~Position() 
{
	delete hdata;
}

pgn::Position& pgn::Position::operator = (const pgn::Position& src) 
{
	if (&src == this)
		return(*this);

	hdata->squares = src.hdata->squares;
	hdata->sideToMove = src.hdata->sideToMove;
	hdata->moveNumber = src.hdata->moveNumber;
	hdata->halfmoveClock = src.hdata->halfmoveClock;

	return *this;
}

// get an empty board
void pgn::Position::clear()
{
 	hdata->sideToMove = pgn::white;
	hdata->moveNumber = 1;
	hdata->halfmoveClock = 0;

	hdata->squares.clear();
	for (int i=0; i<8; ++i)
	  	for (int j=0; j<8; ++j)
			hdata->squares.push_back(pgn::Square('a'+j, '0'+i+1));
}

// get the start chess position
void pgn::Position::reset()
{
	hdata->sideToMove = pgn::white;
	hdata->moveNumber = 1;
	hdata->halfmoveClock = 0;
	hdata->squares.clear();

	// col 1
	hdata->squares.push_back(pgn::Square('a', '1', pgn::Piece::WhiteRook()));
	hdata->squares.push_back(pgn::Square('b', '1', pgn::Piece::WhiteKnight()));
	hdata->squares.push_back(pgn::Square('c', '1', pgn::Piece::WhiteBishop()));
	hdata->squares.push_back(pgn::Square('d', '1', pgn::Piece::WhiteQueen()));
	hdata->squares.push_back(pgn::Square('e', '1', pgn::Piece::WhiteKing()));
	hdata->squares.push_back(pgn::Square('f', '1', pgn::Piece::WhiteBishop()));
	hdata->squares.push_back(pgn::Square('g', '1', pgn::Piece::WhiteKnight()));
	hdata->squares.push_back(pgn::Square('h', '1', pgn::Piece::WhiteRook()));
	
	// col 2
	for (int i=0; i<8; ++i)
		 hdata->squares.push_back(pgn::Square('a'+i, '2', pgn::Piece::WhitePawn()));

	// col 3-6
	for (int i=3; i<7; ++i)
	  	for (int j=0; j<8; ++j)
			hdata->squares.push_back(pgn::Square('a'+j, '0'+i));

	// col 7
	for (int i=0; i<8; ++i)
		 hdata->squares.push_back(pgn::Square('a'+i, '7', pgn::Piece::BlackPawn()));
	
	// col 8
	hdata->squares.push_back(pgn::Square('a', '8', pgn::Piece::BlackRook()));
	hdata->squares.push_back(pgn::Square('b', '8', pgn::Piece::BlackKnight()));
	hdata->squares.push_back(pgn::Square('c', '8', pgn::Piece::BlackBishop()));
	hdata->squares.push_back(pgn::Square('d', '8', pgn::Piece::BlackQueen()));
	hdata->squares.push_back(pgn::Square('e', '8', pgn::Piece::BlackKing()));
	hdata->squares.push_back(pgn::Square('f', '8', pgn::Piece::BlackBishop()));
	hdata->squares.push_back(pgn::Square('g', '8', pgn::Piece::BlackKnight()));
	hdata->squares.push_back(pgn::Square('h', '8', pgn::Piece::BlackRook()));
}

std::string pgn::Position::fen() const
{
	Fen fen(*hdata);
	return fen.str();
}

pgn::Color pgn::Position::sideToMove() const
{
	return hdata->sideToMove;
}

void pgn::Position::setSideToMove(pgn::Color c)
{
	hdata->sideToMove = c;
}

int pgn::Position::moveNumber() const
{
	return hdata->moveNumber;
}

void pgn::Position::setMoveNumber(int n)
{
	hdata->moveNumber = n;
}

int pgn::Position::halfmoveClock() const
{
	return hdata->halfmoveClock;
}

void pgn::Position::setHalfmoveClock(int n)
{
	hdata->halfmoveClock = n;
}

bool pgn::Position::whiteKingSideCastlingAvailable() const
{
	return hdata->whiteKingCastlingAvailable; 
}

bool pgn::Position::whiteQueenSideCastlingAvailable() const
{
	return hdata->whiteQueenCastlingAvailable; 
}

bool pgn::Position::blackKingSideCastlingAvailable() const
{
	return hdata->blackKingCastlingAvailable; 
}

bool pgn::Position::blackQueenSideCastlingAvailable() const
{
	return hdata->blackQueenCastlingAvailable; 
}

pgn::Square pgn::Position::enPassantTargetSquare() const
{
	return hdata->enPassantTargetSquare;
}

pgn::Piece pgn::Position::pieceAt(char col, char row) const
{
	return hdata->pieceAt(col, row);
}

void pgn::Position::setPieceAt(const pgn::Piece &piece, char col, char row)
{
	hdata->setPieceAt(piece, col, row);
}

void pgn::Position::update(pgn::Ply &ply)
{
	if (ply.isShortCastle())
	{
		if (sideToMove() == white)
		{
			setPieceAt(Piece::Null(), 'e', '1');
			setPieceAt(Piece::Null(), 'h', '1');
			setPieceAt(Piece::WhiteKing(), 'g', '1');
			setPieceAt(Piece::WhiteRook(), 'f', '1');
		}
		else
		{
			setPieceAt(Piece::Null(), 'e', '8');
			setPieceAt(Piece::Null(), 'h', '8');
			setPieceAt(Piece::BlackKing(), 'g', '8');
			setPieceAt(Piece::BlackRook(), 'f', '8');
		}
	}
	else if (ply.isLongCastle())
	{
		if (sideToMove() == white)
		{
			setPieceAt(Piece::Null(), 'e', '1');
			setPieceAt(Piece::Null(), 'a', '1');
			setPieceAt(Piece::WhiteKing(), 'c', '1');
			setPieceAt(Piece::WhiteRook(), 'd', '1');
		}
		else
		{
			setPieceAt(Piece::Null(), 'e', '8');
			setPieceAt(Piece::Null(), 'a', '8');
			setPieceAt(Piece::BlackKing(), 'c', '8');
			setPieceAt(Piece::BlackRook(), 'd', '8');
		}
	}
	else // normal move
	{
		hdata->fillFromSquare(ply);
		Square from = ply.fromSquare();
		Square to = ply.toSquare();
		Piece piece;
		if (ply.promotion())
		{
		  
			// work-around for colorless ply pieces.
			piece = Piece(ply.promoted().letter(), hdata->sideToMove);
		}
		else if (hdata->isEnPassant(ply))
		{
			// remove the pawn taken
			if (hdata->sideToMove == white)
				setPieceAt(Piece::Null(), to.col(), to.row()-1);
			else
				setPieceAt(Piece::Null(), to.col(), to.row()+1);
			  
			// work-around for colorless ply pieces.
			piece = Piece(ply.piece().letter(), hdata->sideToMove);
		}
		else
		{
			// work-around for colorless ply pieces.
			piece = Piece(ply.piece().letter(), hdata->sideToMove);

		}
		setPieceAt(Piece::Null(), from.col(), from.row());
		setPieceAt(piece, to.col(), to.row());
		ply.setPiece(piece); 
	}
	if (pieceAt('e', '1') != Piece::WhiteKing())
	{
		hdata->whiteKingCastlingAvailable = false;
		hdata->whiteQueenCastlingAvailable = false;
	}
	if (pieceAt('e', '8') != Piece::BlackKing())
	{
		hdata->blackKingCastlingAvailable = false;
		hdata->blackQueenCastlingAvailable = false;
	}
	if (pieceAt('a', '1') != Piece::WhiteRook())
		hdata->whiteQueenCastlingAvailable = false;
	if (pieceAt('h', '1') != Piece::WhiteRook())
		hdata->whiteKingCastlingAvailable = false;
	if (pieceAt('a', '8') != Piece::BlackRook())
		hdata->blackQueenCastlingAvailable = false;
	if (pieceAt('h', '8') != Piece::BlackRook())
		hdata->blackKingCastlingAvailable = false;
	
	if (hdata->sideToMove == pgn::black) 
	{
		hdata->sideToMove = pgn::white;
		hdata->moveNumber++;
	}
	else
		hdata->sideToMove = pgn::black;

	if (ply.isCapture() || ply.piece() == Piece::Pawn() || ply.piece() == Piece::WhitePawn() || ply.piece() == Piece::BlackPawn())
	{	
		hdata->halfmoveClock = 0;
	}
	else
	{
		hdata->halfmoveClock ++;
	}
	if (ply.piece() == Piece::BlackPawn() && 
		hdata->isDoublePawnMove(ply) && 
		hdata->hasPieceOnSide(ply.toSquare(), Piece::WhitePawn()))
	{
		hdata->enPassantTargetSquare = pgn::Square(ply.fromSquare().col(), ply.fromSquare().row() - 1);
	}
	if (ply.piece() == Piece::WhitePawn() && 
		hdata->isDoublePawnMove(ply) && 
		hdata->hasPieceOnSide(ply.toSquare(), Piece::BlackPawn()))
	{
		hdata->enPassantTargetSquare = pgn::Square(ply.fromSquare().col(), ply.fromSquare().row() + 1);
	}
}


bool pgn::Position::operator == (const pgn::Position& src) const
{
	if (hdata->sideToMove != src.hdata->sideToMove)
		return false;
	
	for (int i=0; i<64; ++i)
		if (hdata->squares[i] != src.hdata->squares[i])
			return false;

	// moveNumber and halfmoveClock intentionally left out.
	
	return true;
}

bool pgn::Position::operator != (const pgn::Position& src) const 
{ 
	return !(*this == src); 
}

pgn::Position::iterator pgn::Position::begin() const
{
	pgn::Position::iterator itr(*this);
	return itr;
}

pgn::Position::iterator pgn::Position::end() const
{
	pgn::Position::iterator itr(*this,0);
	return itr;
}

//
// iterator
//

namespace pgn
{
	struct iteratorData 
	{
		std::vector<pgn::Square>::iterator it;
	};
};


pgn::Position::iterator::iterator()
{
	hdata = new iteratorData;
}

pgn::Position::iterator::iterator(const pgn::Position &p)
{
	hdata = new iteratorData;
	hdata->it = p.hdata->squares.begin();
}

pgn::Position::iterator::iterator(const pgn::Position &p, int)
{
	hdata = new iteratorData;
	hdata->it = p.hdata->squares.end();
}

pgn::Position::iterator::iterator(const pgn::Position::iterator &other)
{
	hdata = new iteratorData;
	hdata->it = other.hdata->it;	
}

pgn::Position::iterator::~iterator()
{
	delete hdata;
}

pgn::Position::iterator& pgn::Position::iterator::operator = (const pgn::Position::iterator& other) 
{
	if (&other == this)
		return *this;

	hdata->it = other.hdata->it;	

	return *this;
}

pgn::Position::iterator& pgn::Position::iterator::operator ++ ()
{
	hdata->it++;
	return (*this);
}

pgn::Position::iterator& pgn::Position::iterator::operator ++ (int)
{
	return (operator ++ ());
}

pgn::Position::iterator::pointer pgn::Position::iterator::operator -> () const
{
	return &(*hdata->it);
}

pgn::Position::iterator::reference pgn::Position::iterator::operator * () const
{
	return *(hdata->it);
}

bool pgn::Position::iterator::operator == (const pgn::Position::iterator& other) const
{
	return  (hdata->it == other.hdata->it);
}

bool pgn::Position::iterator::operator != (const pgn::Position::iterator& other) const
{
	return (!((*this) == other));
}
