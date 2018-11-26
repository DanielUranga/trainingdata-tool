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
#include <PGNException.h> 
#include <fstream> 
#include <iostream> 

//
// Just a minimal improvement over the first example, looking for
// white wins, black wins and draws.
//
int main(int argc, char *argv[])
{
	try
	{
		std::ifstream pgnfile("./sample.pgn");
		pgn::GameCollection games;

		// collecting games from file
		pgnfile >> games;

		// producing a simple report 
		int whiteWins = 0;
		int patte = 0;
		int blackWins = 0;
		for (pgn::GameCollection::iterator itr = games.begin(); itr != games.end(); itr++)
		{
				if (itr->result().isWhiteWin())
					whiteWins++;
				else if (itr->result().isBlackWin())
					blackWins++;
				else if (itr->result().isDrawn())
					patte++;
		}
		std::cout << std::endl;
		std::cout << "games in file sample.pgn : " << games.size() << std::endl << std::endl;
		std::cout << "white wins               : " << whiteWins << std::endl;
		std::cout << "draws                    : " << patte << std::endl;
		std::cout << "black wins               : " << blackWins << std::endl << std::endl;
	}
	catch (std::exception &e)
	{
		std::cerr << "exception: " << e.what() << std::endl;
		return -1;
	}

	return 0;
}

