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

#ifndef PGNGameCollection_h
#define PGNGameCollection_h

#include <fstream>
#include <vector>
#include <PGNGame.h>

namespace pgn
{
	class GameCollection 
	{
		public:
		
			class iterator;
			friend class iterator;

			GameCollection();
			GameCollection(const GameCollection& src);

			virtual ~GameCollection();

			GameCollection& operator = (const GameCollection& src);
			bool operator == (const GameCollection& src) const;
			bool operator != (const GameCollection& src) const;
			Game operator [] (const int idx) const;
			
			void push_back(const Game& src);
			iterator insert(iterator position, const Game& src);
			void insert(iterator position, iterator first, iterator last);
			iterator erase(iterator position);
			iterator erase(iterator first, iterator last);
			int size() const;
			void clear();
			iterator begin();
			iterator end();
				
			friend std::ostream& operator << ( std::ostream& os, const GameCollection& src);
			friend std::istream& operator >> ( std::istream& is, GameCollection& src);

		private:
	
			struct GameCollectionData *hdata;
	};

	class GameCollection::iterator
	{
		public:

			// seems that stl relies on these typedef in many
			// algorithms (remove_copy_if for sure).
			typedef std::forward_iterator_tag iterator_category;
			typedef Game value_type;
			typedef Game* pointer;
			typedef Game& reference;
			typedef int difference_type;
			
			friend class GameCollection;
			
			iterator();
			iterator(const GameCollection &ml);
			iterator(const GameCollection &ml, int);
			iterator(const iterator&);
			~iterator();
			iterator& operator = (const iterator&);
			iterator& operator ++ ();
			iterator& operator ++ (int);
			pointer operator -> () const;
//			const Game& operator * () const;
			reference operator * () const;
			bool operator == (const iterator&) const;
			bool operator != (const iterator&) const;

		private:
		
			struct iteratorData *hdata;
	};
};

#endif 

