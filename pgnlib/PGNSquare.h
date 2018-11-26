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

#ifndef PGNSquare_h
#define PGNSquare_h

#include <PGNPiece.h>
#include <string>

namespace pgn 
{
	class Square 
	{
		public:
		
			Square();
			Square(char col, char row, const Piece &p = Piece::Null());
			Square(const std::string &col_row, const Piece &p = Piece::Null());
			Square(const Square& src);

			virtual ~Square();

			Square& operator = (const Square& src);
			bool operator == (const Square& src) const;
			bool operator != (const Square& src) const;
	
			char col() const; // returns 'a' for a4 square, 0 if undefined
			char row() const; // returns '4' for a4 square, 0 if undefined
			std::string str() const; // returns "a4" for a4 square, "" if undefined
			int colIndex() const; // returns 0 for a4 square
			int rowIndex() const; // returns 3 for a4 square
			Piece piece() const;
			
			bool valid() const; // 'a4' is a valid square. '4', 'a', '' are not.
			bool null() const; // '' is null, both row() and col() return zero.

			friend std::ostream& operator << ( std::ostream& os, const pgn::Square& src);
			friend std::istream& operator >> ( std::istream& is, Square& src);

		private:
	
			char col_;
			char row_;
			Piece piece_;

	};
	
	std::istream& operator >> ( std::istream& is, Square& src);
	std::ostream& operator << ( std::ostream& os, const Square& src);
};

#endif 

