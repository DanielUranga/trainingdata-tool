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

//
// A very basic example in handling move object from a list of games. 
// Using iterators we get each move and we just print it on stdout.
//
int main(int argc, char *argv[])
{
	try
	{
		// get all the games from files into game collection
		std::string fname = "./games.pgn";
		std::ifstream pgnfile(fname.c_str());
		pgn::GameCollection games;
		pgnfile >> games;

		// iterate now trough the collection ...
		for (pgn::GameCollection::iterator itr = games.begin(); itr != games.end(); itr++)
		{
			// ... and get the move list from each game
			pgn::Game game = *itr;
			pgn::MoveList movelist = game.moves();
			
			
			std::cout << std::endl << "---- new game ----" << std::endl << std::endl;
			
			// now let's iterate trough the list of moves and print each one to stdout
			for (pgn::MoveList::iterator itr2 = movelist.begin(); itr2 != movelist.end(); itr2++)
				std::cout << "move: ." << *itr2 << "." << std::endl;
		}
		
	}
	catch (std::exception &e)
	{
		std::cerr << "exception: " << e.what() << std::endl;
		return -1;
	}

	return 0;
}

