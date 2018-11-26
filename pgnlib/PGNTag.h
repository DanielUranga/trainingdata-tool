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

#ifndef PGNTag_h
#define PGNTag_h

#include <fstream>
#include <string>

namespace pgn
{
	class Tag 
	{
		public:
		
			Tag();
			Tag(const Tag& src);
			Tag(const std::string name, const std::string value);

			virtual ~Tag();

			Tag& operator = (const Tag& other);
			bool operator == (const Tag& other) const;
			bool operator != (const Tag& other) const;
			bool operator == (const std::string& tagName) const;
			bool operator != (const std::string& tagName) const;
		
			std::string name() const;
			std::string value() const;

			friend std::ostream& operator << (std::ostream& os, const Tag& src);
			friend std::istream& operator >> (std::istream& is, Tag& src);

		private:

			struct TagData *hdata;
	};

	std::ostream& operator << ( std::ostream& os, const pgn::Tag& src );
	std::istream& operator >> (std::istream& is, Tag& src);

};

#endif 

