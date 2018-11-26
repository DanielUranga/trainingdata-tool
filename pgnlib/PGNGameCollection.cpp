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
#include <algorithm>
#include <PGNGameCollection.h>
#include <PGNTagList.h>
#include <PGNTag.h>
#include <PGNParser.h>

namespace pgn
{
	struct GameCollectionData 
	{
		std::vector<pgn::Game> games;
	};

	struct iteratorData 
	{
		std::vector<pgn::Game>::iterator it;
	};
};

pgn::GameCollection::GameCollection() 
{
	hdata = new pgn::GameCollectionData;
}

pgn::GameCollection::GameCollection(const pgn::GameCollection& src) 
{
	hdata = new pgn::GameCollectionData;
	hdata->games = src.hdata->games;
}

pgn::GameCollection::~GameCollection() 
{
//	std::cout << "delete GameCollectionData " << sizeof(*hdata) << std::endl; 
	delete hdata;
}

pgn::GameCollection& pgn::GameCollection::operator = (const pgn::GameCollection& src) 
{
	if (&src == this)
		return(*this);

	hdata->games = src.hdata->games;

	return *this;
}

bool pgn::GameCollection::operator == (const pgn::GameCollection& src) const
{
	if (hdata->games.size() != src.hdata->games.size())
		return false;

	for (int i=0; i<(int)src.hdata->games.size(); i++)
	{
		if (hdata->games[i] != src.hdata->games[i])
			return false;
	}

	return true;
}

bool pgn::GameCollection::operator != (const pgn::GameCollection& src) const 
{ 
	return !(*this == src); 
}

std::ostream& pgn::operator << ( std::ostream& os, const pgn::GameCollection& src ) 
{
	for (int i=0; i<(int)src.hdata->games.size(); i++)
	{
		os << src.hdata->games[i] << std::endl;
	}
	
	return os;
}

std::istream& pgn::operator >> ( std::istream& is, pgn::GameCollection& src ) 
{
	
	std::string pgndata;
	std::copy(std::istreambuf_iterator<char>(is), std::istreambuf_iterator<char>(), std::inserter(pgndata, pgndata.end()));

	std::string::const_iterator itr1 = pgndata.begin();
	std::string::const_iterator itr2 = pgndata.end();
	
	pgn::Parser parser;
	parser.getGameCollection(itr1, itr2, src);
	return is;
}

void pgn::GameCollection::push_back(const Game& game)
{
	hdata->games.push_back(game);
}
pgn::GameCollection::iterator pgn::GameCollection::insert(pgn::GameCollection::iterator position, const pgn::Game& game)
{
	hdata->games.insert(position.hdata->it, game);	
	return position;
}

void pgn::GameCollection::insert(pgn::GameCollection::iterator position, pgn::GameCollection::iterator first, pgn::GameCollection::iterator last)
{
  	hdata->games.insert(position.hdata->it, first.hdata->it, last.hdata->it);
}
pgn::GameCollection::iterator pgn::GameCollection::erase(pgn::GameCollection::iterator position)
{
    	hdata->games.erase(position.hdata->it);
	return position;
}
pgn::GameCollection::iterator pgn::GameCollection::erase(pgn::GameCollection::iterator first, pgn::GameCollection::iterator last)
{
    	hdata->games.erase(first.hdata->it, last.hdata->it);
	return last;
}


int pgn::GameCollection::size() const
{ 
	return hdata->games.size(); 
}

void pgn::GameCollection::clear() 
{ 
	hdata->games.clear(); 
}

pgn::GameCollection::iterator pgn::GameCollection::begin() 
{ 
	pgn::GameCollection::iterator itr(*this);
	return itr;
}

pgn::GameCollection::iterator pgn::GameCollection::end() 
{ 
	pgn::GameCollection::iterator itr(*this, 0);
	return itr;
}

pgn::Game pgn::GameCollection::operator [] (const int idx) const 
{ 
	return hdata->games[idx]; 
}

//
// iterator
//


pgn::GameCollection::iterator::iterator()
{
	hdata = new iteratorData;
}

pgn::GameCollection::iterator::iterator(const pgn::GameCollection &ml)
{
	hdata = new iteratorData;
	hdata->it = ml.hdata->games.begin();
}

pgn::GameCollection::iterator::iterator(const pgn::GameCollection &ml, int)
{
	hdata = new iteratorData;
	hdata->it = ml.hdata->games.end();
}

pgn::GameCollection::iterator::iterator(const pgn::GameCollection::iterator &other)
{
	hdata = new iteratorData;
	hdata->it = other.hdata->it;	
}

pgn::GameCollection::iterator::~iterator()
{
	delete hdata;
}

pgn::GameCollection::iterator& 
	pgn::GameCollection::iterator::operator = (const pgn::GameCollection::iterator& other) 
{
	if (&other == this)
		return *this;

	hdata->it = other.hdata->it;	

	return *this;
}

pgn::GameCollection::iterator& pgn::GameCollection::iterator::operator ++ ()
{
	hdata->it++;
	return (*this);
}

pgn::GameCollection::iterator& pgn::GameCollection::iterator::operator ++ (int)
{
	return (operator ++ ());
}

pgn::GameCollection::iterator::pointer pgn::GameCollection::iterator::operator -> () const
{
	return &(*hdata->it);
}

//const pgn::Game& pgn::GameCollection::iterator::operator * () const
pgn::GameCollection::iterator::reference pgn::GameCollection::iterator::operator * () const
{
	return *(hdata->it);
}

bool pgn::GameCollection::iterator::operator == (const pgn::GameCollection::iterator& other) const
{
	if (hdata->it == other.hdata->it)
		return true;
	else
		return false;
}

bool pgn::GameCollection::iterator::operator != (const pgn::GameCollection::iterator& other) const
{
	return (!((*this) == other));
}

