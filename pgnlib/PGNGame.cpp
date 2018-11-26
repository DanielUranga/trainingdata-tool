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
#include <iterator>
#include <sstream>
#include <PGNGame.h>
#include <PGNTagList.h>
#include <PGNMoveList.h>
#include <PGNParser.h>

namespace pgn
{
	struct GameData 
	{
		pgn::TagList tags;
		pgn::MoveList moves;
		pgn::GameResult result;
		std::string date;
		std::string white;
		std::string black;
		std::string keyTagName;
		pgn::CommentText bgComment;
		pgn::CommentText agComment;
	};
};

pgn::Game::Game()
{
	hdata = new pgn::GameData;
}

pgn::Game::Game(const TagList &tl, const MoveList &ml, const GameResult &gr)
{
	hdata = new pgn::GameData;
	hdata->tags = tl;
	hdata->moves = ml;
	hdata->result = gr;

	if (tl.find("Date"))
		hdata->date = tl["Date"].value();
	else
		hdata->date = "????.??.??";

	if (tl.find("White"))
		hdata->white = tl["White"].value();
	else
		hdata->white = "";

	if (tl.find("Black"))
		hdata->black = tl["Black"].value();
	else
		hdata->black = "";

	hdata->keyTagName = "Date";
//	std::cout << "new GameData " << sizeof(*hdata) << std::endl; 
}

pgn::Game::Game(const pgn::Game& src) 
{
	hdata = new pgn::GameData;

	hdata->tags = src.hdata->tags;
	hdata->moves = src.hdata->moves;
	hdata->result = src.hdata->result;
	hdata->date = src.hdata->date;
	hdata->white = src.hdata->white;
	hdata->black = src.hdata->black;
	hdata->keyTagName = src.hdata->keyTagName;
//	std::cout << "new GameData " << sizeof(*hdata) << std::endl; 
}

pgn::Game& pgn::Game::operator = (const pgn::Game& src) 
{
	if (&src == this)
		return(*this);

	hdata->tags = src.hdata->tags;
	hdata->moves = src.hdata->moves;
	hdata->result = src.hdata->result;
	hdata->date = src.hdata->date;
	hdata->white = src.hdata->white;
	hdata->black = src.hdata->black;
	hdata->keyTagName = src.hdata->keyTagName;
	
	return *this;
}

pgn::Game::~Game() 
{
//	std::cout << "deleting GameData " << sizeof(*hdata) << std::endl; 
	delete hdata;
}

void pgn::Game::bindBeforeGameComment(const CommentText &ct)
{
	hdata->bgComment = ct;
}

void pgn::Game::bindAfterGameComment(const CommentText &ct)
{
	hdata->agComment = ct;
}

bool pgn::Game::operator == (const pgn::Game& src) const
{
	if (hdata->tags != src.hdata->tags)
		return false;
	if (hdata->moves != src.hdata->moves)
		return false;
	if (hdata->result != src.hdata->result)
		return false;
	if (hdata->date != src.hdata->date)
		return false;
	if (hdata->white != src.hdata->white)
		return false;
	if (hdata->black != src.hdata->black)
		return false;

	return true;
}

std::ostream& pgn::operator << ( std::ostream& os, const pgn::Game& src ) 
{
	os << src.hdata->tags;
	os << std::endl;
	if (src.hdata->moves.size()) // empty game is possible
		os << src.hdata->moves;
	os << src.hdata->result;
	os << std::endl;

	return os;
}

std::istream& pgn::operator >> ( std::istream& is, pgn::Game& src ) 
{
	std::string pgndata;
	std::copy(std::istreambuf_iterator<char>(is), std::istreambuf_iterator<char>(), std::inserter(pgndata, pgndata.end()));

	std::string::const_iterator itr1 = pgndata.begin();
	std::string::const_iterator itr2 = pgndata.end();
	
	pgn::Parser parser;
	parser.getGame(itr1, itr2, src);
	return is;
}

bool pgn::Game::operator != (const pgn::Game& src) const 
{ 
	return !(*this == src); 
}
		
pgn::TagList& pgn::Game::tags() const
{
	return hdata->tags;
}

pgn::MoveList& pgn::Game::moves() const
{
	return hdata->moves;
}

std::string pgn::Game::date() const 
{ 
	return hdata->date; 
}

std::string pgn::Game::white() const 
{ 
	return hdata->white; 
}

std::string pgn::Game::black() const 
{ 
	return hdata->black; 
}

pgn::GameResult pgn::Game::result() const 
{ 
	return hdata->result; 
}

