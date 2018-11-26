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

#ifndef PGNCommentText_h
#define PGNCommentText_h

#include <fstream>
#include <string>

namespace pgn
{
	class CommentText 
	{
		public:
			
			CommentText();
			CommentText(const std::string &s);
			CommentText(const CommentText& src);

			virtual ~CommentText();

			CommentText& operator = (const CommentText& src);
			bool operator == (const CommentText& src) const;
			bool operator != (const CommentText& src) const;

			friend std::ostream& operator << ( std::ostream& os, const CommentText& src);

		private:

			struct CommentTextData *hdata;

	};
	std::ostream& operator << ( std::ostream& os, const pgn::CommentText& src );
};

#endif 

