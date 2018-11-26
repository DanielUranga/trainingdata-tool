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
#include <PGNCommentText.h>

namespace pgn
{
	struct CommentTextData 
	{
		std::string comment;		
	};
};

pgn::CommentText::CommentText() 
{
	hdata = new pgn::CommentTextData;
}

pgn::CommentText::CommentText(const std::string &text) 
{
	hdata = new pgn::CommentTextData;
	hdata->comment = text;
}

pgn::CommentText::CommentText(const pgn::CommentText& src) 
{
	hdata = new pgn::CommentTextData;
	hdata->comment = src.hdata->comment;
}

pgn::CommentText::~CommentText() 
{
	delete hdata;
}

pgn::CommentText& pgn::CommentText::operator = (const pgn::CommentText& src) 
{
	if (&src == this)
		return(*this);

	hdata->comment = src.hdata->comment;

	return *this;
}

bool pgn::CommentText::operator == (const pgn::CommentText& src) const
{
	return (hdata->comment == src.hdata->comment);
}

bool pgn::CommentText::operator != (const pgn::CommentText& src) const
{
	return !(*this == src); 
}

std::ostream& pgn::operator << ( std::ostream& os, const pgn::CommentText& src ) 
{
	if (src.hdata->comment == "")
		return os;

	os << " {" << src.hdata->comment << "}";

	return os;
}

