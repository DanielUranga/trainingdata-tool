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

#ifndef PGNGame_h
#define PGNGame_h

#include <fstream>
#include <PGNGameResult.h>
#include <PGNTagList.h>
#include <PGNMoveList.h>

namespace pgn
{
	class Game 
	{
		public:
		
			Game();
			Game(const TagList &tl, const MoveList &ml, const GameResult &gr);
			Game(const Game& src);

			virtual ~Game();

			Game& operator = (const Game& src);
			bool operator == (const Game& src) const;
			bool operator != (const Game& src) const;
		
			TagList& tags() const;
			MoveList& moves() const;
			std::string date() const;
			std::string white() const;
			std::string black() const;
			GameResult result() const;
			void bindBeforeGameComment(const CommentText &ct);
			void bindAfterGameComment(const CommentText &ct);
		
			friend std::ostream& operator << ( std::ostream& os, const Game& src);
			friend std::istream& operator >> ( std::istream& is, Game& src);

		private:

			struct GameData *hdata;
	};
	
};

#endif 

