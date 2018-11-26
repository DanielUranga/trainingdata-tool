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
#include <iostream>
#include <iterator>
#include <PGNSquare.h>

pgn::Square::Square()
:col_(0),row_(0),piece_(pgn::Piece::Null())
{
//	std::cout << "new Square " << sizeof(*this) << std::endl; 
}

pgn::Square::Square(char col, char row, const pgn::Piece &piece)
:col_(col),row_(row),piece_(piece)
{
}

pgn::Square::Square(const std::string &col_row, const pgn::Piece &piece)
:col_(col_row.at(0)), row_(col_row.at(1)), piece_(piece)
{
	piece_ = piece;
	if (col_row.empty())
	{
		col_ = 0;
		row_ = 0;
	}
	else
	{
		col_ = col_row.at(0);
		row_ = col_row.at(1);
	}
}

pgn::Square::Square(const pgn::Square& src)
:col_(src.col_),row_(src.row_),piece_(src.piece_)
{
//	std::cout << "new Square " << sizeof(*this) << std::endl; 
}

pgn::Square::~Square()
{ 
//	std::cout << "deleting Square " << sizeof(*this) << std::endl; 
}

bool pgn::Square::valid() const
{
	return (col() != 0) && (row() != 0);
}

bool pgn::Square::null() const
{
	return (col() == 0) && (row() == 0);
}

pgn::Square& pgn::Square::operator = (const pgn::Square& src)
{
	if (&src == this)
		return (*this);

	col_ = src.col_;
	row_ = src.row_;
	piece_ = src.piece_;

	return(*this);
}

bool pgn::Square::operator == (const pgn::Square& src) const
{
	return (col_ == src.col_) && (row_ == src.row_) && (piece_ == src.piece_);
}

char pgn::Square::col() const
{
	return col_;
}

char pgn::Square::row() const
{
	return row_;
}

std::string pgn::Square::str() const
{
	if (null()) 
		return "";
	else
	{
		std::stringstream ss;
		ss << col() << row();
		return ss.str();
	}
}

pgn::Piece pgn::Square::piece() const
{
	return piece_;
}

bool pgn::Square::operator != (const pgn::Square& src) const 
{
	return !(src == *this);
}

//
// these methods remap board coordinates as follows
//
// 8  . . . . . . . .     ---->      7  . . . . . . . . 
// 7  . . . . . . . .                6  . . . . . . . . 
// 6  . . . . . . . .                5  . . . . . . . . 
// 5  . . . . . . . .     ---->      4  . . . . . . . . 
// 4  . . . . . . . .     ---->      3  . . . . . . . . 
// 3  . . . . . . . .                2  . . . . . . . . 
// 2  . . . . . . . .                1  . . . . . . . . 
// 1  . . . . . . . .     ---->      0  . . . . . . . . 
//                                 
//    a b c d e f g h                   0 1 2 3 4 5 6 7
//
int pgn::Square::colIndex() const
{
	if (col_ == 0)
	    return -1;
	
	return (col_ - 'a');
}
int pgn::Square::rowIndex() const
{
	if (row_ == 0)
	    return -1;

	return (row_ - '1');
}

std::ostream& pgn::operator << ( std::ostream& os, const pgn::Square& src ) 
{
	if (src.col() != 0)
		os << src.col();

	if (src.row() != 0)
		os << src.row();
	
	return os;
}

std::istream& pgn::operator >> ( std::istream& is, pgn::Square& src ) 
{
	std::string str;
	std::copy(std::istreambuf_iterator<char>(is), std::istreambuf_iterator<char>(), std::inserter(str, str.end()));
	src.col_ = str[0];
	src.row_ = str[1];
	return is;
}
