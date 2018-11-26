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

#ifndef PGNPly_h
#define PGNPly_h

#include <fstream>
#include <string>
#include <vector>
#include <PGNPiece.h> 
#include <PGNSquare.h> 
#include <PGNCommentText.h> 

namespace pgn
{
	class MoveList;
	class Ply 
	{
		public:

			Ply(const Ply& src);
//			Ply(const std::string &ply_text, const Square &from = Square());
			Ply(const std::string &ply_text);
			Ply();

			virtual ~Ply();

			Ply& operator = (const Ply& src);
			bool operator == (const Ply& src) const;
			bool operator != (const Ply& src) const;

			std::string str() const;
			bool isLongCastle() const;
			bool isShortCastle() const;
			bool isCapture() const;
			bool isCheck() const;
			bool isCheckMate() const; 
			void bindComment(const CommentText &comment);
			void unbindComment();
			void bindVariation(const MoveList &variation);
			void unbindVariations();
			void setGlyphValue(int val);
			int glyphValue() const;

			Square fromSquare() const; // if O-O or O-O-O returns a null square
			Square toSquare() const; // if O-O or O-O-O returns a null square
			void setFromSquare(const Square &s);
			void setToSquare(const Square &s);
			Piece piece() const;
			void setPiece(const Piece &p);
			bool promotion() const;
			Piece promoted() const;
			
			bool valid() const;

			friend std::ostream& operator << ( std::ostream& os, const pgn::Ply& src);
			friend std::istream& operator >> ( std::istream& is, Ply& src);


		private:

			struct PlyData *hdata;
	};
	
	std::istream& operator >> ( std::istream& is, Ply& src);
	std::ostream& operator << ( std::ostream& os, const Ply& src);
};

#endif 

