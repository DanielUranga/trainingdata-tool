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
#include <sstream>
#include <string>
#include <stdexcept>
#include <PGNGameResult.h>

pgn::GameResult::GameResult() 
:result_('*')
{
}

pgn::GameResult::GameResult(std::string s) 
{
	if (s == "1/2-1/2")
	{
		result_ = 'x';
	}
	else if (s == "1-0")
	{
		result_ = '1';
	}
	else if (s == "0-1")
	{
		result_ = '0';
	}
	else if (s == "*")
	{
		result_ = '*';
	}
	else
	{
		std::ostringstream err;
		err << "Invalid Result: >" << s << "<";
		throw std::runtime_error(err.str());
	}
//	std::cout << "new GameResult " << sizeof(*this) << std::endl; 
}

pgn::GameResult::GameResult(const pgn::GameResult& src) 
{
	result_ = src.result_;
}

pgn::GameResult& pgn::GameResult::operator = (const pgn::GameResult& src) 
{
	if (&src == this)
		return(*this);

	result_ = src.result_;

	return *this;
}

bool pgn::GameResult::operator == (const pgn::GameResult& src) const
{
	return result_ == src.result_;
}

std::ostream& pgn::operator << ( std::ostream& os, const pgn::GameResult& src ) 
{
	switch (src.result_)
	{
		case '1':
			os << "1-0";
	  		break;
		case '0':
			os << "0-1";
	  		break;
		case 'x':
			os << "1/2-1/2";
	  		break;
		case '*':
		default:
			os << "*";
	  		break;
	}
	return os;
}

bool pgn::GameResult::operator != (const pgn::GameResult& src) const 
{ 
	return !(*this == src); 
}

bool pgn::GameResult::isWhiteWin() const 
{ 
	return result_ == '1'; 
}

bool pgn::GameResult::isBlackWin() const 
{ 
	return result_ == '0'; 
}

bool pgn::GameResult::isDrawn() const 
{ 
	return result_ == 'x'; 
}

bool pgn::GameResult::isUnknown() const 
{ 
	return result_ == '*'; 
}

