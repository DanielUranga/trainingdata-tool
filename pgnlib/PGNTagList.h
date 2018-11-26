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

#ifndef PGNTagList_h
#define PGNTagList_h

#include <fstream>
#include <vector>
#include <PGNTag.h>

namespace pgn
{
	class TagList 
	{
		public:
		
			class iterator;
			friend class iterator;

			TagList();
			TagList(const TagList& src);

			virtual ~TagList();

			TagList& operator = (const TagList& src);
			bool operator == (const TagList& src) const;
			bool operator != (const TagList& src) const;
			Tag operator [] (const std::string& tagName) const;

			bool find(std::string tagName) const;
			void insert(const Tag& src);
			void erase(const Tag& src);
			int size() const;
			iterator begin() const;
			iterator end() const;
		
			friend std::ostream& operator << ( std::ostream& os, const TagList& src);
			friend std::istream& operator >> ( std::istream& is, TagList& target);

		private:

			struct TagListData *hdata;
	};

	class TagList::iterator
	{
		public:
			// seems that stl relies on these typedef in many
			// algorithms (remove_copy_if for sure).
			typedef std::forward_iterator_tag iterator_category;
			typedef Tag value_type;
			typedef Tag* pointer;
			typedef Tag& reference;
			typedef int difference_type;
			
			iterator();
			iterator(const TagList &tl);
			iterator(const TagList &tl, int);
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

