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

#include <sstream>
#include <iostream>
#include <iterator>
#include <PGNMoveList.h>
#include <PGNParser.h>

#include <iomanip>

namespace pgn
{
	struct MoveListData 
	{
		std::vector<pgn::Move> mosse;
		pgn::CommentText comment;
	};

	struct iteratorData 
	{
		std::vector<pgn::Move>::iterator it;
	};
};
		
pgn::MoveList::MoveList() 
{
	hdata = new pgn::MoveListData;
//	std::cout << "new MoveListData " << sizeof(*hdata) << std::endl; 
}

pgn::MoveList::MoveList(const pgn::MoveList& src) 
{
	hdata = new pgn::MoveListData;
	hdata->mosse = src.hdata->mosse;
}

pgn::MoveList::~MoveList() 
{
//	std::cout << "deleting MoveListData " << sizeof(*hdata) << std::endl; 
	delete hdata;
}

pgn::MoveList& pgn::MoveList::operator = (const pgn::MoveList& src) 
{
	if (&src == this)
		return(*this);

	hdata->mosse = src.hdata->mosse;

	return *this;
}

void pgn::MoveList::push_back(const pgn::Move& src)
{
	hdata->mosse.push_back(src);
}
pgn::MoveList::iterator pgn::MoveList::insert(pgn::MoveList::iterator position, const pgn::Move& move)
{
	hdata->mosse.insert(position.hdata->it, move);	
	return position;
}

void pgn::MoveList::insert(pgn::MoveList::iterator position, pgn::MoveList::iterator first, pgn::MoveList::iterator last)
{
  	hdata->mosse.insert(position.hdata->it, first.hdata->it, last.hdata->it);
}
pgn::MoveList::iterator pgn::MoveList::erase(pgn::MoveList::iterator position)
{
    	hdata->mosse.erase(position.hdata->it);
	return position;
}
pgn::MoveList::iterator pgn::MoveList::erase(pgn::MoveList::iterator first, pgn::MoveList::iterator last)
{
    	hdata->mosse.erase(first.hdata->it, last.hdata->it);
	return last;
}

bool pgn::MoveList::operator == (const pgn::MoveList& src) const
{
	if (hdata->mosse.size() != src.hdata->mosse.size())
		return false;

	for (int i=0; i<(int)src.hdata->mosse.size(); i++)
	{
		if (hdata->mosse[i] != src.hdata->mosse[i])
			return false;
	}

	return true;
}

bool pgn::MoveList::find(const pgn::Move& src) const
{
	for (int i=0; i<(int)hdata->mosse.size(); i++)
	{
		if (hdata->mosse[i] == src)
			return true;
	}
	
	return false;
}

void pgn::MoveList::bindComment(const CommentText &ct)
{
	hdata->comment = ct;
}

//
// output buffer formatting algorithm, from newsgroup alt.comp.lang.learn.c-c++
// Author: Dietmar Kuehl (thank you!)
//
#include <streambuf>
class formatbuf: public std::streambuf
{
	public:
		formatbuf(int length, std::streambuf* sbuf);
		~formatbuf();

	private:
		int overflow(int c);

		std::streambuf* m_sbuf;
};

formatbuf::formatbuf(int length, std::streambuf* sbuf)
{
	m_sbuf = sbuf;
	char *buf = new char [length + 1];
	setp(buf, buf + length);
}

formatbuf::~formatbuf()
{
	m_sbuf->sputn(pbase(), pptr() - pbase());
	m_sbuf->sputc('\n');
	delete [] pbase();
}

int formatbuf::overflow(int c)
{
	// 
	// pptr() ritorna il puntatore alla posizione corrente nel buffer.
	// epptr() ritorna il puntatore alla fine del buffer.
	// se i due non coincidono c'e` ancora spazio nel buffer e non bisogna fare
	// niente
	//
	if (pptr() != epptr())
		return 0;
	else
	{
		//
		// il carattere in overflow viene aggiunto in fondo al buffer.
		//
		*epptr() = c;

		//
		// partendo dal fondo e andando a ritroso cerca il primo carattere
		// newline ('\n') o spazio (' ')
		// la gestione del newline non era presente nel codice originale che 
		// ho copiato da usenet ma e` necessaria perche` in fondo questa 
		// routine ha lo scopo di forzare un newline ogni N caratteri e non 
		// prima, ma dei newline possono esistere nel flusso di caratteri
		// processato (ad esempio nella formattazione dei commenti tra {})
		// e ovviamente vanno rispettati.
		// Quindi pre prima cosa verifico se esiste un newline e in questo
		// caso non devo porre alcuna interruzione di linea 'artificiale',
		// solo se la riga ne e` priva cerco l'ultimo spazio e proseguo con
		// il codice originale della routine.
		//
		char *ptr = epptr();

		while ((ptr != pbase()) && (*ptr != '\n'))
			--ptr;

		if (ptr == pbase()) // no newlines in the text
		{
			ptr = epptr();
			while ((ptr != pbase()) && (*ptr != ' '))
				--ptr;

			//
			// e` arrivato all'inizio del buffer senza trovare spazi: riposiziona
			// il puntatore alla fine del buffer
			//
			if (ptr == pbase()) // no space on the line
				ptr = epptr();
		}
		//
		// spara in output il contenuto del buffer dall'inizio fino all'ultimo
		// spazio (trovato in precedenza), oppure il contenuto dell'intero
		// buffer se non ha trovato spazi. quindi va a capo.
		//
		m_sbuf->sputn(pbase(), ptr - pbase());
		m_sbuf->sputc('\n');

		//
		// Se effettivamente ptr puntava ad uno spazio lo sposto avanti di un
		// carattere (lo spazio e` stato rimpiazzato nel flusso di output dal
		// '\n' e non lo devo riscrivere.
		//
//		if (*ptr == ' ')
		if ((*ptr == ' ') || (*ptr == '\n'))
			++ptr;

		// setta la posizione currente del buffer all'inizio del buffer stesso.
		// quindi shifta il contenuto del buffer a partire da ptr
		setp(pbase(), epptr());
		sputn(ptr, epptr() + 1 - ptr);
	}
	return c;
}

std::ostream& pgn::operator << ( std::ostream& os, const pgn::MoveList& src ) 
{
	// limit line length to 80 chars. fmt prints a newline when meets the last
	// whitespace in the line
	formatbuf fmt(80, os.rdbuf());
	std::ostream out(&fmt);
	for (int i=0; i<(int)src.hdata->mosse.size()-1; i++)
	{
		out << src.hdata->mosse[i] << " ";
	}
	out << src.hdata->mosse[src.hdata->mosse.size()-1];

	return os;
}

std::istream& pgn::operator >> ( std::istream& is, pgn::MoveList& src ) 
{
	std::string pgndata;
	std::copy(std::istreambuf_iterator<char>(is), std::istreambuf_iterator<char>(), std::inserter(pgndata, pgndata.end()));

	std::string::const_iterator itr1 = pgndata.begin();
	std::string::const_iterator itr2 = pgndata.end();
	
	pgn::Parser parser;
	parser.getMoveList(itr1, itr2, src);
	return is;
}

bool pgn::MoveList::operator != (const pgn::MoveList& src) const 
{ 
	return !(*this == src); 
}

int pgn::MoveList::size() const 
{ 
	return hdata->mosse.size(); 
}

pgn::Move pgn::MoveList::operator [] (int idx) 
{ 
	return hdata->mosse[idx]; 
}		

pgn::MoveList::iterator pgn::MoveList::begin() const
{
	pgn::MoveList::iterator itr(*this);
	return itr;
}

pgn::MoveList::iterator pgn::MoveList::end() const
{
	pgn::MoveList::iterator itr(*this,0);
	return itr;
}

//
// iterator
//

pgn::MoveList::iterator::iterator()
{
	hdata = new iteratorData;
}

pgn::MoveList::iterator::iterator(const pgn::MoveList &ml)
{
	hdata = new iteratorData;
	hdata->it = ml.hdata->mosse.begin();
}

pgn::MoveList::iterator::iterator(const pgn::MoveList &ml, int)
{
	hdata = new iteratorData;
	hdata->it = ml.hdata->mosse.end();
}

pgn::MoveList::iterator::iterator(const pgn::MoveList::iterator &other)
{
	hdata = new iteratorData;
	hdata->it = other.hdata->it;	
}

pgn::MoveList::iterator::~iterator()
{
	delete hdata;
}

pgn::MoveList::iterator& pgn::MoveList::iterator::operator = (const pgn::MoveList::iterator& other) 
{
	if (&other == this)
		return *this;

	hdata->it = other.hdata->it;	

	return *this;
}

pgn::MoveList::iterator& pgn::MoveList::iterator::operator ++ ()
{
	hdata->it++;
	return (*this);
}

pgn::MoveList::iterator& pgn::MoveList::iterator::operator ++ (int)
{
	return (operator ++ ());
}

pgn::MoveList::iterator::pointer pgn::MoveList::iterator::operator -> () const
{
	return &(*hdata->it);
}

pgn::MoveList::iterator::reference pgn::MoveList::iterator::operator * () const
{
	return *(hdata->it);
}

bool pgn::MoveList::iterator::operator == (const pgn::MoveList::iterator& other) const
{
	return  (hdata->it == other.hdata->it);
}

bool pgn::MoveList::iterator::operator != (const pgn::MoveList::iterator& other) const
{
	return (!((*this) == other));
}
