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

#include <iostream>
#include <iterator>
#include <sstream>
#include <stdexcept>
#include <PGNMove.h>
#include <PGNParser.h>

namespace pgn
{
	struct MoveData 
	{
		bool complete;
		int number;
		pgn::Ply white;
		pgn::Ply black;
	};
};

pgn::Move::Move() 
{
	hdata = new pgn::MoveData;
	
	hdata->complete = false;
	hdata->number=0;
}

pgn::Move::Move(const pgn::Move& src) 
{
	hdata = new pgn::MoveData;

	hdata->complete = src.hdata->complete;
	hdata->number = src.hdata->number;
	hdata->white = src.hdata->white;
	hdata->black = src.hdata->black;
}

pgn::Move& pgn::Move::operator = (const pgn::Move& src) 
{
	if (&src == this)
		return(*this);

	hdata->complete = src.hdata->complete;
	hdata->number = src.hdata->number;
	hdata->white = src.hdata->white;
	hdata->black = src.hdata->black;

	return *this;
}

pgn::Move::Move(const pgn::Ply &white, const pgn::Ply &black, int number)
{
	hdata = new pgn::MoveData;

	hdata->number = number;
	hdata->white = white;
	hdata->black = black;
	hdata->complete = (hdata->white.valid() && hdata->black.valid());
//	std::cout << "new MoveData " << sizeof(*hdata) << std::endl;
}

pgn::Move::~Move() 
{
//	std::cout << "deleting MoveData " << sizeof(*hdata) << std::endl; 
	// set pointers to null after segfault due to double dellocation. 
	// some hidden problem somewhere ...
	delete hdata; hdata = 0;
}

bool pgn::Move::operator == (const pgn::Move& src) const
{
	if (hdata->complete != src.hdata->complete)
		return false;
	if (hdata->number != src.hdata->number)
		return false;
	if (hdata->white != src.hdata->white)
		return false;
	if (hdata->black != src.hdata->black)
		return false;

	return true;
}

std::ostream& pgn::operator << ( std::ostream& os, const pgn::Move& src ) 
{
	if (src.hdata->complete)
		os << src.hdata->number << ". " << src.hdata->white << " " << src.hdata->black;
	else if (src.hdata->white.valid())
		os << src.hdata->number << ". " << src.hdata->white;
	else if (src.hdata->black.valid())
		os << src.hdata->number << "... " << src.hdata->black;
	return os;
}


std::istream& pgn::operator >> ( std::istream& is, pgn::Move& src ) 
{
	std::string str;
	std::copy(std::istreambuf_iterator<char>(is), std::istreambuf_iterator<char>(), std::inserter(str, str.end()));
	pgn::Parser parser;
	std::string::const_iterator itr1 = str.begin();
	std::string::const_iterator itr2 = str.end();
	parser.getMove(itr1, itr2, src);
	return is;
}


bool pgn::Move::isCheckMate() const
{
	return (hdata->white.isCheckMate()) || (hdata->black.isCheckMate());
}

bool pgn::Move::operator != (const pgn::Move& src) const 
{ 
	return !(*this == src); 
}

pgn::Ply pgn::Move::white() const 
{ 
	return hdata->white; 
} 

pgn::Ply pgn::Move::black() const 
{ 
	return hdata->black; 
}

