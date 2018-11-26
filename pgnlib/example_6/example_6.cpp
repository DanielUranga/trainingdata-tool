//
// Copyright (C) 2002 Andrea Vinzoni <vinzo@planet.it>
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

#include <PGNGameCollection.h> 
#include <fstream> 
#include <iostream>
#include <map>

//
// README:
//
// so you get all the moves, but what to do with them? What about some silly statistics?
// Let's count how many moves each type of piece does, from a file full of games.
//

// update a counter for a piece based on the ply received.
void udpate_piece_counters(std::map<std::string, int> &counters, const pgn::Ply &ply)
{
	// a castle move is usually considered a king move
	if (ply.isLongCastle() || ply.isShortCastle())
	{
		counters["king"] += 1;
		return;
	}
	
	switch (ply.piece().letter())
	{
		 case 'K':
			counters["king"] += 1;
			break;
		 case 'Q':
			counters["queen"] += 1;
			break;
		 case 'R':
			counters["rook"] += 1;
			break;
		 case 'B':
			counters["bishop"] += 1;
			break;
		 case 'N':
			counters["knight"] += 1;
			break;
		 case 'P':
			counters["pawn"] += 1;
			break;
	}
}

int main(int argc, char *argv[])
{
	// we need a bunch of counters
	int ply_count = 0;
	int game_count = 0;
	std::map<std::string, int> piece_move_counters;
	piece_move_counters["king"] = 0;
	piece_move_counters["queen"] = 0;
	piece_move_counters["rook"] = 0;
	piece_move_counters["bishop"] = 0;
	piece_move_counters["knight"] = 0;
	piece_move_counters["pawn"] = 0;
	
	try
	{
		// get the games, iterate trough the moves,... ok, we already know this stuff.
		std::string fname = "./games.pgn";
		std::ifstream pgnfile(fname.c_str());
		pgn::GameCollection games;
		pgnfile >> games;

		for (pgn::GameCollection::iterator itr = games.begin(); itr != games.end(); itr++)
		{
			game_count++;

			pgn::Game game = *itr;
			pgn::MoveList movelist = game.moves();
			
			pgn::Ply ply;
			for (pgn::MoveList::iterator itr2 = movelist.begin(); itr2 != movelist.end(); itr2++)
			{
				// just update the counters for each ply read (white and black)
				ply = itr2->white();
				udpate_piece_counters(piece_move_counters, ply);
				ply_count++;

				ply = itr2->black();
				if (!ply.valid()) break;
				udpate_piece_counters(piece_move_counters, ply);
				ply_count++;
			}
		}
		
		//
		// producing the report with number of moves played by each piece.
		//
		
		float percentage = 0.0;
		
		std::cout << std::endl;
		std::cout << "Game file                      : " << fname << std::endl;
		std::cout << std::endl;
		std::cout << "Total games processed          : " << game_count << std::endl;
		std::cout << "Total moves processed          : " << ply_count << std::endl;
		std::cout << std::endl;
		percentage = 100.0 / ply_count * piece_move_counters["king"];
		std::cout << "King moves processed           : " << piece_move_counters["king"] << " (" << percentage << "%)" << std::endl;

		percentage = 100.0 / ply_count * piece_move_counters["queen"];
		std::cout << "Queen moves processed          : " << piece_move_counters["queen"] << " (" << percentage << "%)" << std::endl;

		percentage = 100.0 / ply_count * piece_move_counters["rook"];
		std::cout << "Rook moves processed           : " << piece_move_counters["rook"] << " (" << percentage << "%)" << std::endl;

		percentage = 100.0 / ply_count * piece_move_counters["bishop"];
		std::cout << "Bishop moves processed         : " << piece_move_counters["bishop"] << " (" << percentage << "%)" << std::endl;

		percentage = 100.0 / ply_count * piece_move_counters["knight"];
		std::cout << "Knight moves processed         : " << piece_move_counters["knight"] << " (" << percentage << "%)" << std::endl;

		percentage = 100.0 / ply_count * piece_move_counters["pawn"];
		std::cout << "Pawn moves processed           : " << piece_move_counters["pawn"] << " (" << percentage << "%)" << std::endl;
		std::cout << std::endl;
	}
	catch (std::exception &e)
	{
		std::cerr << "exception: " << e.what() << " after " << ply_count << " moves processed " << std::endl;
		return -1;
	}

	return 0;
}

