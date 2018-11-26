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
#include <PGNTagList.h> 
#include <fstream> 
#include <iostream> 

//
// The main reason I started to program pgnlib was to use 
// it to manage my own pgn game files.
//
// Among them the file that automatically collects my games from Free
// Internet Chess Server (FICS) is probably the more difficult to sort
// out (as it keeps growing day by day). 
//
// This is the first step: I get all my blitz games (two minutes for the
// whole game) and quasi-serious games (2 minutes for the game, plus 12 
// seconds per move [fischer clock] and save them in separate places.
// All the rest (analysis on winboard, games by others ficsians, exotic
// time controls, etc) I put in "others" (maybe a more sensible place could
// be /dev/null).
//
int main(int argc, char *argv[])
{
	try
	{
		// reading the whole stuff from the big file 

		std::cout << std::endl << "loading games from file fics.pgn ... " << std::endl;

		std::ifstream pgnfile("./games.pgn");
		pgn::GameCollection games;
		pgnfile >> games;

		std::cout << std::endl << "searching blitz and lightning games ..." << std::endl;
		
		// creating recipient files.
		std::ofstream my_2_0 ("./my2-0.pgn");
		std::ofstream my_2_12 ("./my2-12.pgn");
		std::ofstream other_games("./others.pgn");

		// browsing the game collection
		pgn::GameCollection::iterator itr;
		for (itr = games.begin(); itr != games.end(); itr++)
		{
			// I tell you a secret: topogigio, an italian cartoon character, 
			// is my nick on fics
			if ((itr->white() == "topogigio") || (itr->black() == "topogigio"))
			{
				// discard all games without time control tag
				if (itr->tags().find("TimeControl") == false)
				{
					other_games << *itr;
					continue;
				}
				// discard aborted games as well
				if ((itr->tags()["Result"].value()) == "*")
				{
					other_games << *itr;
					continue;
				}

				// save lightning and blitz games in different files
				if ((itr->tags()["TimeControl"].value()) == "120")
				{
					my_2_0 << *itr;
				}
				else if (itr->tags()["TimeControl"].value() == "120+12")
				{
					my_2_12 << *itr;
				}
				// discard all the games played with different time control
				else
				{
					other_games << *itr;
				}
			}
			// discard the garbage
			else
			{
				other_games << *itr;
			}
		}

		std::cout << std::endl;
		std::cout << "blitz games saved in file my_2-12.pgn" << std::endl;
		std::cout << "lightning games saved in file my_2-0.pgn" << std::endl;
		std::cout << "other games saved in file others.pgn" << std::endl;
		std::cout << std::endl;
	}
	catch (std::exception &e)
	{
		std::cerr << "exception: " << e.what() << std::endl;
		return -1;
	}

	return 0;
}

