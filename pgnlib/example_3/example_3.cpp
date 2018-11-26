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
#include <PGNGame.h> 
#include <PGNMoveList.h> 
#include <PGNException.h> 
#include <iostream> 
#include <fstream> 

//
// Select all the miniatures (games won in 20 moves or less) from the
// sample file and save them in another file.
// 
int main(int argc, char *argv[])
{
	try
	{
		std::ifstream pgnfile("./games.pgn");
		pgn::GameCollection games;

		// collecting games from file
		pgnfile >> games;

		// searching the games and adding to collection (wins only, no
		// grandmaster draws!).
		pgn::GameCollection miniatures;
		for (pgn::GameCollection::iterator itr = games.begin(); itr != games.end(); itr++)
		{
				if ((itr->moves().size() <= 20) &&
					((itr->result().isWhiteWin()) || 
					 (itr->result().isBlackWin())))
				{
					
					miniatures.push_back(*itr);
				}
		}

		// counting miniatures
		std::cout << "found " << miniatures.size() << " miniatures in file games.pgn" << std::endl;
		std::cout << std::endl;

		// copy the games found in a new pgn file
		std::ofstream ofs("miniatures.pgn");
		ofs << miniatures;
	}
	catch(std::exception &e)
	{
		std::cerr << "exception: " << e.what() << std::endl;
	}

	return 0;
}

