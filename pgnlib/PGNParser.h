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

#ifndef PGNParser_h
#define PGNParser_h

#include <string>

namespace pgn
{
	class GameCollection;
	class Game;
	class MoveList;
	class Move;
	class Ply;
	class GameResult;
	class CommentText;
	class TagList;
	class Tag;
	
	class Parser
	{
		public:

			Parser():plyCount_(0),moveCount_(0),gameCount_(0) {}

			bool getGameCollection(std::string::const_iterator &itr1, 
											const std::string::const_iterator &itr2, pgn::GameCollection &out);
			bool getGame(std::string::const_iterator &itr1, 
											const std::string::const_iterator &itr2, pgn::Game &out);

			bool getMoveList(std::string::const_iterator &itr1, 
											const std::string::const_iterator &itr2, pgn::MoveList &out);
			bool getMove(std::string::const_iterator &itr1, 
											const std::string::const_iterator &itr2, pgn::Move &out);
			bool getPly(std::string::const_iterator &itr1, 
											const std::string::const_iterator &itr2, pgn::Ply &out);
			bool getTagList(std::string::const_iterator &itr1, 
											const std::string::const_iterator &itr2, pgn::TagList &out);
			bool getTag(std::string::const_iterator &itr1, 
											const std::string::const_iterator &itr2, pgn::Tag &out);
			bool getGameResult(std::string::const_iterator &itr1, 
											const std::string::const_iterator &itr2, pgn::GameResult &out);
			bool getComment(std::string::const_iterator &itr1, 
											const std::string::const_iterator &itr2, pgn::CommentText &out);
			bool getMoveNumber(std::string::const_iterator &itr1, 
											const std::string::const_iterator &itr2, std::string &out, int &dotsCount);
			
			int getGlyph(std::string::const_iterator &itr1, const std::string::const_iterator &itr2);

			bool getVariation(std::string::const_iterator &itr1, 
											const std::string::const_iterator &itr2, pgn::MoveList &out);


			unsigned long plyCount() const { return plyCount_; }
			unsigned long moveCount() const { return moveCount_; }
			unsigned long gameCount() const { return gameCount_; }

		private:

			static void skipBlanks(std::string::const_iterator &itr1, const std::string::const_iterator &end);

			unsigned long plyCount_;
			unsigned long moveCount_;
			unsigned long gameCount_;

	};
}
#endif // PGNParser_h
