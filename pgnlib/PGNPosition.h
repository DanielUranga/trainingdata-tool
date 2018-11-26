//
// Copyright (C) 2013 Andrea Vinzoni <vinzoni@users.sourceforge.net>
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

#ifndef PGNPosition_h
#define PGNPosition_h

#include "PGNSquare.h"
#include "PGNPly.h"
#include "PGNPiece.h"
#include <string> 
#include <exception>

namespace pgn
{
	//
	// FEN notation partially supported (don't rely on values for en_passant square and halfmove_clock) 
	//
	class Position 
	{
		public:
			class iterator;
			friend class iterator;
			
			Position();
			Position(const Position& src);
			Position(const std::string& fen);

			virtual ~Position();

			Position& operator = (const Position& src);
			bool operator == (const Position& src) const;
			bool operator != (const Position& src) const;
		
			void clear(); // clear to an empty board
			void reset(); // reset to the initial chess position
			
			std::string fen() const;
			
			Color sideToMove() const;
			void setSideToMove(Color c);
			
			int moveNumber() const;
			void setMoveNumber(int n);

			// how many half-moves without captures and pawn moves (for 50 moves draw rule)
			int halfmoveClock() const;
			void setHalfmoveClock(int n);

			// castling rights
			bool whiteKingSideCastlingAvailable() const;
			bool whiteQueenSideCastlingAvailable() const;
			bool blackKingSideCastlingAvailable() const;
			bool blackQueenSideCastlingAvailable() const;

			Square enPassantTargetSquare() const;

			Piece pieceAt(char col, char row) const;
			void setPieceAt(const Piece &p, char col, char row);
			
			void update(Ply &ply);
			
			iterator begin() const;
			iterator end() const;

		private:

			struct PositionData *hdata;
	};
	
	class Position::iterator
	{
		public:
			// seems that stl relies on these typedef in many
			// algorithms (remove_copy_if for sure).
			typedef std::forward_iterator_tag iterator_category;
			typedef Square value_type;
			typedef Square* pointer;
			typedef Square& reference;
			typedef int difference_type;
			
			iterator();
			iterator(const Position &ml);
			iterator(const Position &ml, int);
			iterator(const iterator&);
			~iterator();
			iterator& operator = (const iterator&);
			iterator& operator ++ ();
			iterator& operator ++ (int);
			pointer operator -> () const;
			reference operator * () const;
			bool operator == (const iterator&) const;
			bool operator != (const iterator&) const;

		private:
		
			struct iteratorData *hdata;
	};
};

#endif 

