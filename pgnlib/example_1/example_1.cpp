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
#include <iostream> 
#include <fstream> 

//
// This is a sort of "hello world" program for pgnlib: reading all the games
// from a sample (and simple) pgn file, then counting them.
//
int main(int argc, char *argv[])
{
	try
	{
		std::ifstream pgnfile("./sample.pgn");
		pgn::GameCollection games;

		// collecting games from file
		pgnfile >> games;

		// now counting and printing the result
		std::cout << std::endl << "The sample.pgn file contains " << games.size() << " games." 
				<< std::endl << std::endl;

	}
	catch (std::exception &e)
	{
		std::cerr << "exception: " << e.what() << std::endl;
		return -1;
	}

	return 0;
}

