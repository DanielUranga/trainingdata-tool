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

#ifndef PGNMoveList_h
#define PGNMoveList_h

#include <fstream>
#include <vector>
#include <set>
#include <PGNMove.h>
#include <PGNGameResult.h>

namespace pgn
{
	class MoveList 
	{
		public:

			class iterator;
			friend class iterator;
		
			MoveList();
			MoveList(const MoveList& src);

			virtual ~MoveList();

			MoveList& operator = (const MoveList& src);
			bool operator == (const MoveList& src) const;
			bool operator != (const MoveList& src) const;
			Move operator [] (int idx);

			bool find(const Move& src) const;
			
			void push_back(const Move& src);
			iterator insert(iterator position, const Move& src);

			void insert(iterator position, iterator first, iterator last);
			iterator erase(iterator position);
			iterator erase(iterator first, iterator last);

			
			int size() const;

			iterator begin() const;
			iterator end() const;

			void bindComment(const CommentText &ct);
		
			friend std::ostream& operator << ( std::ostream& os, const MoveList& src);
			friend std::istream& operator >> ( std::istream& is, MoveList& src);

		private:

			struct MoveListData *hdata;
		
	};

	class MoveList::iterator
	{
		public:
		  
			// seems that stl relies on these typedef in many
			// algorithms (remove_copy_if for sure).
			typedef std::forward_iterator_tag iterator_category;
			typedef Move value_type;
			typedef Move* pointer;
			typedef Move& reference;
			typedef int difference_type;

			friend class MoveList;

			iterator();
			iterator(const MoveList &ml);
			iterator(const MoveList &ml, int);
			iterator(const iterator&);
			~iterator();
			iterator& operator = (const iterator&);
			iterator& operator ++ ();
			iterator& operator ++ (int);
			pointer operator -> () const;
			reference operator * () const;
			bool operator == (const iterator&) const;
			bool operator != (const iterator&) const;

		private:
		
			struct iteratorData *hdata;
	};
};

#endif 

