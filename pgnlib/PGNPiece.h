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

#ifndef PGNPiece_h
#define PGNPiece_h

namespace pgn
{
	typedef enum { white, black, undef } Color;
	
	class Piece 
	{
		public:

			static const Piece Null();
			static const Piece Pawn();
			static const Piece Knight();
			static const Piece Bishop();
			static const Piece Rook();
			static const Piece Queen();
			static const Piece King();
			static const Piece WhitePawn();
			static const Piece WhiteKnight();
			static const Piece WhiteBishop();
			static const Piece WhiteRook();
			static const Piece WhiteQueen();
			static const Piece WhiteKing();
			static const Piece BlackPawn();
			static const Piece BlackKnight();
			static const Piece BlackBishop();
			static const Piece BlackRook();
			static const Piece BlackQueen();
			static const Piece BlackKing();

			Piece();
			Piece(const Piece &src);
			Piece(const char letter, Color c = undef);

			Piece& operator = (const Piece& src);
			bool operator == (const Piece& src) const;
			bool operator != (const Piece& src) const; 

			char letter() const; // returns uppercase piece letter ('P' for pawn, '0' for null)
			Color color() const;
			
			friend std::ostream& operator << ( std::ostream& os, const pgn::Piece& src);

		private:
	
			char data_;
			
	};
	std::ostream& operator << ( std::ostream& os, const pgn::Piece& src );
};

#endif 

