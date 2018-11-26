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

#include <cstdio>
#include <cassert>
#include <sstream>
#include <iostream>
#include <PGNPiece.h>

//
// data scheme:
// '0' --> null piece
// '1' --> pawn (color unspecified)
// '2' --> knight (color unspecified)
// '3' --> bishop (color unspecified)
// '4' --> rook (color unspecified)
// '5' --> queen (color unspecified)
// '6' --> king (color unspecified)
// 'P' --> white pawn
// 'N' --> white knight
// 'B' --> white bishop
// 'R' --> white rook
// 'Q' --> white queen
// 'K' --> white king
// 'p' --> black pawn
// 'n' --> black knight
// 'b' --> black bishop
// 'r' --> black rook
// 'q' --> black queen
// 'k' --> black king
//

//
// Passing always uppercase letters in ctor (as you read them in pgn stream)
// specify the color as second argument.
//
const pgn::Piece pgn::Piece::Null() { return pgn::Piece('0'); }
const pgn::Piece pgn::Piece::Pawn() { return pgn::Piece('P'); }
const pgn::Piece pgn::Piece::Knight() { return pgn::Piece('N'); }
const pgn::Piece pgn::Piece::Bishop() { return pgn::Piece('B'); }
const pgn::Piece pgn::Piece::Rook() { return pgn::Piece('R'); }
const pgn::Piece pgn::Piece::Queen() { return pgn::Piece('Q'); }
const pgn::Piece pgn::Piece::King() { return pgn::Piece('K'); }
const pgn::Piece pgn::Piece::WhitePawn() { return pgn::Piece('P', white); }
const pgn::Piece pgn::Piece::WhiteKnight() { return pgn::Piece('N', white); }
const pgn::Piece pgn::Piece::WhiteBishop() { return pgn::Piece('B', white); }
const pgn::Piece pgn::Piece::WhiteRook() { return pgn::Piece('R', white); }
const pgn::Piece pgn::Piece::WhiteQueen() { return pgn::Piece('Q', white); }
const pgn::Piece pgn::Piece::WhiteKing() { return pgn::Piece('K', white); }
const pgn::Piece pgn::Piece::BlackPawn() { return pgn::Piece('P', black); }
const pgn::Piece pgn::Piece::BlackKnight() { return pgn::Piece('N', black); }
const pgn::Piece pgn::Piece::BlackBishop() { return pgn::Piece('B', black); }
const pgn::Piece pgn::Piece::BlackRook() { return pgn::Piece('R', black); }
const pgn::Piece pgn::Piece::BlackQueen() { return pgn::Piece('Q', black); }
const pgn::Piece pgn::Piece::BlackKing() { return pgn::Piece('K', black); }

// By default build a null piece colorless
pgn::Piece::Piece()
{
	data_='0'; // null piece
	//	std::cout << "new Piece " << sizeof(*this) << std::endl; 
}

pgn::Piece::Piece(const pgn::Piece &src)
{
	data_ = src.data_;
	//	std::cout << "new Piece " << sizeof(*this) << std::endl; 
}

pgn::Piece& pgn::Piece::operator = (const pgn::Piece& src) 
{
	if (&src == this)
		return(*this);

	data_ = src.data_;
	
	return *this;
}

pgn::Piece::Piece(const char letter, Color c)
{
	if (letter == '0') // null piece
	{
		data_ = '0';
		return;
	}
	
	switch (c)
	{
		case undef:
			if ((letter == 'p') || (letter == 'P')) data_ = '1';
			if ((letter == 'n') || (letter == 'N')) data_ = '2';
			if ((letter == 'b') || (letter == 'B')) data_ = '3';
			if ((letter == 'r') || (letter == 'R')) data_ = '4';
			if ((letter == 'q') || (letter == 'Q')) data_ = '5';
			if ((letter == 'k') || (letter == 'K')) data_ = '6';
			break;
		case white:
			if ((letter == 'p') || (letter == 'P')) data_ = 'P';
			if ((letter == 'n') || (letter == 'N')) data_ = 'N';
			if ((letter == 'b') || (letter == 'B')) data_ = 'B';
			if ((letter == 'r') || (letter == 'R')) data_ = 'R';
			if ((letter == 'q') || (letter == 'Q')) data_ = 'Q';
			if ((letter == 'k') || (letter == 'K')) data_ = 'K';
			break;
		case black:
			if ((letter == 'p') || (letter == 'P')) data_ = 'p';
			if ((letter == 'n') || (letter == 'N')) data_ = 'n';
			if ((letter == 'b') || (letter == 'B')) data_ = 'b';
			if ((letter == 'r') || (letter == 'R')) data_ = 'r';
			if ((letter == 'q') || (letter == 'Q')) data_ = 'q';
			if ((letter == 'k') || (letter == 'K')) data_ = 'k';
			break;
	}
}

bool pgn::Piece::operator == (const pgn::Piece& src) const
{
	return (data_ == src.data_);
}

bool pgn::Piece::operator != (const pgn::Piece& src) const 
{
	return !(src == *this);
}

char pgn::Piece::letter() const 
{ 
	if (data_ == '0')
		return '0'; // null piece letter
	
	switch (color())
	{
		case undef:
			if (data_ == '1') return 'P';
			if (data_ == '2') return 'N';
			if (data_ == '3') return 'B';
			if (data_ == '4') return 'R';
			if (data_ == '5') return 'Q';
			if (data_ == '6') return 'K';
			break;
		default:
			return toupper(data_);
			break;
	}
	return '0'; // if everything else fails returns a null piece letter
}

pgn::Color pgn::Piece::color() const
{ 
	if ((data_ >= 'b') && (data_ <= 'r'))
		return black;
	if ((data_ >= 'B') && (data_ <= 'R'))
		return white;
	return undef;
}

std::ostream& pgn::operator << ( std::ostream& os, const pgn::Piece& src ) 
{
	if (src.letter() != 'P')
		os << src.letter();
	return os;
}
