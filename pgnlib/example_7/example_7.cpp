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
#include <PGNPosition.h> 
#include <sstream> 
#include <iostream> 

//
// README:
//
// The idea here is to convert a pgn game in a old-fashioned algebrical complete notation,
// expressing moves not as "1. e4 e5" but as "1.e2-e4 e7-e5".
//
// This is not as straightforward as it may seem because we have very little information about the 
// starting square of the piece on the move in the pgn text.
//
// So we need to be supported by the (new) Position class. First we build the game from text
// the usual game, then we feed each move to a Position object that fills the starting Square
// object for us.
//


// print a minimal (and rather ugly) game header, just the names of the player.
void game_header_printout(const pgn::Game &game)
{
	// get infos from pgn tag list ...
	pgn::TagList tags = game.tags();
	
	std::string white_player = tags["White"].value();
	std::string black_player = tags["Black"].value();
	
	// ... and print them out
	std::cout << std::endl << std::endl << "------------- " << white_player << " - " << black_player << " -------------" << std::endl << std::endl;
}

// this is where we convert and (print it on stdout) the pgn format for the ply
// into the 'complete albebrical notation' format
void move_printout(const pgn::Ply &ply)
{
	// castling output is the same in pgn or algebrical.
	if (ply.isLongCastle() || ply.isShortCastle())
		std::cout << ply << " ";
	else // manage normal moves conversione
		std::cout << ply.piece() << ply.fromSquare() 
			  << (ply.isCapture() ? "x" : "-") 
			  << ply.toSquare() 
			  << (ply.isCheckMate() ? "#" : " ");
}

int main(int argc, char *argv[])
{
	try
	{
		// this time we just hard-coded a single game, instead of reading it from file.
		std::string pgn_game = 
			  "[Event \"Olympiad w\"]\n"
			  "[Site \"Bled SLO\"]\n"
			  "[Date \"2002.10.26\"]\n"
			  "[Round \"1\"]\n"
			  "[White \"Dimovski,A\"]\n"
			  "[Black \"Zhao Xue\"]\n"
			  "[Result \"0-1\"]\n"
			  "[WhiteElo \"2100\"]\n"
			  "[BlackElo \"2367\"]\n"
			  "[ECO \"B90\"]\n"
			  "\n"
			  "1. e4 c5 2. Nf3 d6 3. d4 cxd4 4. Nxd4 Nf6 5. Nc3 a6 6. Nb3 e6 7. Bd3 Be7 8.\n"
			  "O-O Nbd7 9. f4 b5 10. Qe2 Qc7 11. Bd2 Bb7 12. Nd1 Nc5 13. Nf2 d5 14. e5\n"
			  "Nfe4 15. Ba5 Qc8 16. Nd4 O-O 17. Ng4 Kh8 18. Rf3 f5 19. exf6 gxf6 20. Rh3\n"
			  "Nxd3 21. Qxd3 Bc5 22. Ne3 e5 23. fxe5 fxe5 24. Nf3 Qxh3 25. gxh3 Rxf3 0-1\n\n";

		// put it into the game object in the usual way.
		pgn::Game the_game;
		std::stringstream ss;
		ss << pgn_game; 
		ss >> the_game;

		// write down the name of the players extracting infos from the_game object
		game_header_printout(the_game);

		// the customary MoveList object
		pgn::MoveList movelist = the_game.moves();
		// and the new Position object, by default initialized with the initial piece arrangement
		pgn::Position p;

		// now get each move from the game and 
		// 1. fill it up
		// 2. print it out
		for (pgn::MoveList::iterator itr = movelist.begin(); itr != movelist.end(); itr++)
		{
			std::cout << p.moveNumber() << ".";

			// get the white ply, feed it to the position, get it back completed and print it.
			pgn::Ply ply;
			ply = itr->white();
			// Position::update() method has a double purpose:
			// 1. updates the position object
			// 2. complete the 'from square' ply as a side effect
			p.update(ply);
			// this converts from pgn to algebrical notation and print
			move_printout(ply);

			// same stuff for black move
			ply = itr->black();
			if (!ply.valid()) break; // in case the game ends with a white move (not in this game)
			p.update(ply);
			move_printout(ply);
			
			// six moves for every line, please
			if ((p.moveNumber()-1) % 6 == 0) 
				std::cout << std::endl;
		}
		// who won?
		std::cout << the_game.result() << std::endl << std::endl;
	}
	catch (std::exception &e)
	{
		std::cerr << "exception: " << e.what() << std::endl;
		return -1;
	}

	return 0;
}
