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
#include <PGNTag.h>
#include <PGNParser.h>

namespace pgn
{
	struct TagData 
	{
		std::string tagName;
		std::string tagValue;
	};
};

pgn::Tag::Tag() 
{
	hdata = new pgn::TagData;
}

pgn::Tag::Tag(const pgn::Tag& src) 
{
	hdata = new pgn::TagData;
	hdata->tagName = src.hdata->tagName;
	hdata->tagValue = src.hdata->tagValue;
}

//
// pgn::Tag("White", "A.Karpov")
//
pgn::Tag::Tag(const std::string name, const std::string value)
{
	hdata = new pgn::TagData;
	hdata->tagName = name;
	hdata->tagValue = value;
//	std::cout << "new TagData " << sizeof(*hdata) << std::endl; 
}

pgn::Tag::~Tag() 
{
//	std::cout << "deleting TagData " << sizeof(*hdata) << std::endl; 
	delete hdata;
}

pgn::Tag& pgn::Tag::operator = (const pgn::Tag& src) 
{
	if (&src == this)
		return(*this);

	hdata->tagName = src.hdata->tagName;
	hdata->tagValue = src.hdata->tagValue;

	return *this;
}

bool pgn::Tag::operator == (const pgn::Tag& other) const
{
	if (hdata->tagName != other.hdata->tagName)
		return false;
	if (hdata->tagValue != other.hdata->tagValue)
		return false;

	return true;
}

bool pgn::Tag::operator == (const std::string& tagName) const
{
	return (hdata->tagName == tagName);
}

std::ostream& pgn::operator << ( std::ostream& os, const pgn::Tag& src ) 
{
	os << "[" << src.hdata->tagName << " \"" << src.hdata->tagValue << "\"]";
	return os;
}

std::istream& pgn::operator >> ( std::istream& is, pgn::Tag& src ) 
{
	std::string str;
	std::copy(std::istreambuf_iterator<char>(is), std::istreambuf_iterator<char>(), std::inserter(str, str.end()));
	pgn::Parser parser;
	std::string::const_iterator itr1 = str.begin();
	std::string::const_iterator itr2 = str.end();
	parser.getTag(itr1, itr2, src);
	return is;
}

bool pgn::Tag::operator != (const pgn::Tag& src) const 
{ 
	return !(*this == src); 
}

bool pgn::Tag::operator != (const std::string& other) const 
{ 
	return !(*this == other); 
}

std::string pgn::Tag::name() const 
{ 
	return hdata->tagName; 
}

std::string pgn::Tag::value() const 
{ 
	return hdata->tagValue; 
}

