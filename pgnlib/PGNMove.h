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

#ifndef PGNMove_h
#define PGNMove_h

#include <fstream>
#include <PGNPly.h>

namespace pgn 
{
	class Move 
	{
		public:
		
			Move(const Move& src);
			Move();
			Move(const Ply &white, const Ply &black, int number);

			virtual ~Move();

			Move& operator = (const Move& src);
			bool operator == (const Move& src) const;
			bool operator != (const Move& src) const;

			bool isCheckMate() const;
			Ply white() const;
			Ply black() const;
		
			friend std::istream& operator >> ( std::istream& is, Move& src);
			friend std::ostream& operator << ( std::ostream& os, const Move& src);

		private:
	
			struct MoveData *hdata;
	};

	std::istream& operator >> ( std::istream& is, Move& src);
	std::ostream& operator << ( std::ostream& os, const Move& src);

};

#endif 

