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

#ifndef PGNGameResult_h
#define PGNGameResult_h

#include <fstream>
#include <string>

namespace pgn
{
	class GameResult 
	{
		public:
		
			GameResult();
			GameResult(std::string s);
			GameResult(const GameResult& src);

			GameResult& operator = (const GameResult& src);
			bool operator == (const GameResult& src) const;
			bool operator != (const GameResult& src) const;

			bool isWhiteWin() const;
			bool isBlackWin() const;
			bool isDrawn() const;
			bool isUnknown() const;

			friend std::ostream& operator << ( std::ostream& os, const GameResult& src);

		private:

			char result_;
	};

	std::ostream& operator << ( std::ostream& os, const pgn::GameResult& src );
};

#endif 

