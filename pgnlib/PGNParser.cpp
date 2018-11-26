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

#include <iostream>
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <cstdlib>

#include "PGNCommentText.h"
#include "PGNGame.h"
#include "PGNGameCollection.h"
#include "PGNGameResult.h"
#include "PGNMove.h"
#include "PGNMoveList.h"
#include "PGNPiece.h"
#include "PGNPly.h"
#include "PGNSquare.h"
#include "PGNTag.h"
#include "PGNTagList.h"
#include "PGNParser.h"
#include "PGNException.h"

void pgn::Parser::skipBlanks(std::string::const_iterator &itr1, const std::string::const_iterator &end)
{
	while ((itr1 != end) && (isspace(*itr1)))
	{
		itr1++;
	}
}

bool pgn::Parser::getGameCollection(std::string::const_iterator &itr1, const std::string::const_iterator &itr2, pgn::GameCollection &out)
{
	std::string::const_iterator local_itr = itr1;

	pgn::Game game;

	while (getGame(local_itr, itr2, game))
	{
		itr1 = local_itr;
		out.push_back(game);
		gameCount_++;
//		std::cout << "\rgames parsed: " << gameCount_;
//		std::cout.flush();
//		std::cout << "correctly parsed game: " << game.white() << "-" << game.black() << std::endl;
		
	}
//	std::cout << std::endl;

	return true;
}

bool pgn::Parser::getGame(std::string::const_iterator &itr1, const std::string::const_iterator &itr2, pgn::Game &out)
{
	std::string::const_iterator start_game_itr = itr1;
	std::string::const_iterator local_itr = itr1;

	try
	{
		pgn::TagList taglist;
		if (!getTagList(local_itr, itr2, taglist))
			return 0;

		itr1 = local_itr;

		skipBlanks(local_itr, itr2);	
		pgn::CommentText bgComment;
		bool hasBgComment = false;
		if (getComment(local_itr, itr2, bgComment))
		{
			hasBgComment = true;
			itr1 = local_itr;
		}
	
		skipBlanks(local_itr, itr2);	
		pgn::MoveList  movelist;
		if (!getMoveList(local_itr, itr2, movelist))
			throw std::runtime_error("Error parsing movelist"); 

		itr1 = local_itr;

		pgn::GameResult gameResult;
		if (!getGameResult(local_itr, itr2, gameResult))
			return 0;

		itr1 = local_itr;

		out = pgn::Game(taglist, movelist, gameResult);

		skipBlanks(local_itr, itr2);	
		pgn::CommentText agComment;
		bool hasAgComment = false;
		if (getComment(local_itr, itr2, agComment))
		{
			hasAgComment = true;
			itr1 = local_itr;
		}

		if (hasBgComment) out.bindBeforeGameComment(bgComment);
		if (hasAgComment) out.bindAfterGameComment(agComment);
		return true;
	}
	catch (pgn::parse_exception &pe)
	{
		pe.bindParsingText(std::string(start_game_itr, local_itr));
		throw;
	}
	catch (std::exception &e)
	{
		std::ostringstream err;
		err << e.what() << ". I was parsing this game:" << std::string(start_game_itr, local_itr);
		throw std::runtime_error(err.str());
	}
	return 0;
}

bool pgn::Parser::getTagList(std::string::const_iterator &itr1, const std::string::const_iterator &itr2, pgn::TagList &out)
{
	std::string::const_iterator local_itr = itr1;

	pgn::TagList taglist;
	pgn::Tag tag;
	while (getTag(local_itr, itr2, tag))
	{
		itr1 = local_itr;
		taglist.insert(tag);
	}
	if (taglist.size() == 0)
		return false;

	out = taglist;
	return true;
}

bool pgn::Parser::getTag(std::string::const_iterator &itr1, const std::string::const_iterator &itr2, pgn::Tag &out)
{
	std::string::const_iterator local_itr = itr1;
	std::string::const_iterator start_tag = itr1;

	// cerco '['
	skipBlanks(local_itr, itr2);	
	if (*local_itr != '[')
	{
		return false;
	}
	local_itr++;

	// cerco '<tagname>'
	skipBlanks(local_itr, itr2);	
	std::string tagname;

	while ((local_itr != itr2) && (isalnum(*local_itr)))
	{
		tagname += *local_itr++;
	}
	itr1 = local_itr;

	// ricerco il <tagvalue>: devo poter gestire i seguenti casi
	// 1. tag vuoto: [Event ""]
	// 2. tag normale: [Event "Match"]
	// 3. tag 'escaped': [Event "\"Match\""]
	// 4. tag 'escaped' spurio: [Event "Match \"] (ho trovato dei casi)
	//
	// Procedo cosi`: carico in tagvalue tutto quello che leggo tutto fino al ']'
	// Alla fine tagvalue e` validato solo se inizia e termina col carattere '"'

	// carichiamo quindi tagvalue finche` non arriviamo a fine stream (anomalia) o leggiamo il ']' di chiusura
	skipBlanks(local_itr, itr2);	
	std::string tagvalue;
	while ((local_itr != itr2) && (*local_itr != ']'))
	{
		tagvalue += *local_itr++;
	}
	
	// ora validiamo tagvalue
	if (local_itr == itr2) // missing ']'
	{
	    throw pgn::invalid_tag(std::string(start_tag, local_itr));
	}
	if (tagvalue[0] != '"' || tagvalue[tagvalue.size()-1] != '"')
	{
	    throw pgn::invalid_tag(std::string(start_tag, local_itr)); // missing '"'
	}
	
	// elimino le double quotes iniziale e finale dal valore di tagvalue
	tagvalue = tagvalue.substr(1,tagvalue.size()-2);
	
	local_itr++;
	out = pgn::Tag(tagname, tagvalue);
	itr1 = local_itr;
	return true;
}

bool pgn::Parser::getMoveList(std::string::const_iterator &itr1, const std::string::const_iterator &itr2, pgn::MoveList &out)
{
	std::string::const_iterator local_itr = itr1;
	
	pgn::MoveList ml;
	
	// It's possibile that a move list (i.e. a variation) starts with a comment
	pgn::CommentText comment;
	if (getComment(local_itr, itr2, comment))
	{
		skipBlanks(local_itr, itr2);	
		itr1 = local_itr;
		ml.bindComment(comment);
	}

	skipBlanks(local_itr, itr2);	

	pgn::Move move;

 	while ((getMove(local_itr, itr2, move)))
	{
		itr1 = local_itr;
		ml.push_back(move);
	}
	itr1 = local_itr;
	out = ml; 

	return true;
}

bool pgn::Parser::getMove(std::string::const_iterator &itr1, const std::string::const_iterator &itr2, pgn::Move &src)
{
	std::string::const_iterator local_itr = itr1;

	// looking for move number 
	std::string movenumber;
	int dotsCount;
	if (! getMoveNumber(local_itr, itr2, movenumber, dotsCount))
		return false;
	itr1 = local_itr;

	// possibilita`:
	//
	// <move_number><dots> <ply> <result>
	// <move_number><dots> <ply> <move_number>
	// <move_number><dots> <ply> <ply> <result>
	// <move_number><dots> <ply> <ply> <move_number>
 
	// looking for first ply (mandatory)
	skipBlanks(local_itr, itr2);	
	pgn::Ply firstPly;
	if (!getPly(local_itr, itr2, firstPly))
		throw std::runtime_error("Error parsing move"); 

	skipBlanks(local_itr, itr2);	
	pgn::Ply secondPly;
	
	// eof as in 
	// ss << "1.Nf3"; ss >> move;
	if (local_itr == itr2)
	{
		src = pgn::Move(firstPly, secondPly, atoi(movenumber.c_str()));
		moveCount_++;
		return true;
	}
		
	// second ply, game result or another move number?
	// a ply always begins with an uppercase or lowercase alphabetic char.
	// digits and '*' have lesser ascii values. 
	// if we meet a "[" that's owned by the next game, so game result is
	// missing
	if (*local_itr > '9' && *local_itr != '[')
	{
		// looking for second ply (optional)
		getPly(local_itr, itr2, secondPly);
		skipBlanks(local_itr, itr2);	
	}
	itr1 = local_itr;

	src = pgn::Move(firstPly, secondPly, atoi(movenumber.c_str()));
	moveCount_++;
	
	return true;
}

bool pgn::Parser::getGameResult(std::string::const_iterator &itr1, const std::string::const_iterator &itr2, pgn::GameResult &out)
{
	std::string::const_iterator local_itr = itr1;

	std::string result;
	while (!isspace(*local_itr) && local_itr != itr2)
	{
		result += *local_itr++;
	}

	if (result[0] == '[')
	{
		throw pgn::missing_result(); 
	}
	else if ( (result != "1-0") &&
		 (result != "0-1") &&
		 (result != "1/2-1/2") &&
		 (result != "*"))
	{
		throw pgn::invalid_result(); 
	}

	out = result;
	itr1 = local_itr;
	return true;
}

bool pgn::Parser::getMoveNumber(std::string::const_iterator &itr1, const std::string::const_iterator &itr2, std::string &out, int &dotsCount)
{
	std::string::const_iterator local_itr = itr1;

	if (!isdigit(*local_itr))
		return false;

	// looking for number 
	std::string movenumber;
	while (isdigit(*local_itr) && local_itr != itr2)
	{
		movenumber += *local_itr++;
	}
	skipBlanks(local_itr, itr2);	

	if (*local_itr != '.')
	{
		// maybe reading game result '1-0'.
		return false;
	}

	// skipping and counting dots
	dotsCount=0;	
	while ((*local_itr == '.') && (local_itr != itr2))
	{
		local_itr++;
		dotsCount++;
	}
	itr1 = local_itr;
	out = movenumber;
	return true;
}

bool pgn::Parser::getPly(std::string::const_iterator &itr1, const std::string::const_iterator &itr2, pgn::Ply &out)
{
	std::string::const_iterator local_itr = itr1;

	skipBlanks(local_itr, itr2);	
	while ((local_itr != itr2) && (!isspace(*local_itr)))
	{
	  if (*local_itr != ')') // caso "Nf4)"
	    local_itr++;
	  else
	    break;
	}
	out = pgn::Ply(std::string(itr1, local_itr));

	skipBlanks(local_itr, itr2);	
	itr1 = local_itr;

	int glyphval = getGlyph(local_itr, itr2);
	if (glyphval >= 0)
	{
		skipBlanks(local_itr, itr2);	
		itr1 = local_itr;
		out.setGlyphValue(glyphval);
	}
	
	itr1 = local_itr;
	
	pgn::CommentText comment;
	if (getComment(local_itr, itr2, comment))
	{
		skipBlanks(local_itr, itr2);	
		itr1 = local_itr;
		out.bindComment(comment);
	}

	itr1 = local_itr;

	pgn::MoveList variation;
	while (getVariation(local_itr, itr2, variation))
	{
		skipBlanks(local_itr, itr2);	
		itr1 = local_itr;
		out.bindVariation(variation);
	}

	itr1 = local_itr;

	plyCount_++;
	return true;
}

bool pgn::Parser::getComment(std::string::const_iterator &itr1, const std::string::const_iterator &itr2, pgn::CommentText &out)
{
	std::string::const_iterator local_itr = itr1;
	std::string comment;
	if (*local_itr != '{')
		return false;
	local_itr++; // skipping '{'

	while ((*local_itr != '}') && (local_itr != itr2))
	{
		comment += *local_itr++;
	}
	local_itr++; // skipping '}'
	skipBlanks(local_itr, itr2);	
	itr1 = local_itr;
	out = pgn::CommentText(comment);
	return true;
}

bool pgn::Parser::getVariation(std::string::const_iterator &itr1, const std::string::const_iterator &itr2, pgn::MoveList &out)
{
  	std::string::const_iterator local_itr = itr1;
	if (*local_itr != '(')
		return false;
	local_itr++; // skipping '('
	
	if (!getMoveList(local_itr, itr2, out))
		throw std::runtime_error("Error parsing movelist"); 

	local_itr++; // skipping ')'
	skipBlanks(local_itr, itr2);	
	itr1 = local_itr;
	
	return true;
}

int pgn::Parser::getGlyph(std::string::const_iterator &itr1, const std::string::const_iterator &itr2)
{
	if (*itr1 == '$')
	{
		int val = -1;
		itr1++;
		if (isdigit(*itr1))
		  val = *itr1 - '0';
		itr1++;
		if (isdigit(*itr1))
		  val = val*10 + (*itr1 - '0');
		return val;
	}
	else
		return -1;
}

