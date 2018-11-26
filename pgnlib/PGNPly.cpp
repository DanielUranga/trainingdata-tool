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

#include <cstdio>
#include <sstream>
#include <iostream>
#include <iterator>
#include <stdexcept>
#include <cstdlib>
#include <PGNPly.h>
#include <PGNCommentText.h>
#include <PGNMoveList.h>
#include <PGNException.h>
#include <PGNParser.h> 

#include <iostream>

namespace pgn 
{
	struct PlyData 
	{
		std::string text;
		char fromsquare_letter;
		char fromsquare_number;
		Piece piece;
			
		PlyData()
		:fromsquare_letter(0),fromsquare_number(0)
		{}
		
		virtual ~PlyData() {};
		
		virtual PlyData* clone() const = 0;
		
		//
		// I chose to not consider fromsquare_letter and fromsquare_number to test for equality.
		// To me "Nf6" is equal to "Nf6", in a pgn sense, regardless from where the knight come from.
		//
		virtual bool operator == (const PlyData& other) const
		{
			  if (text != other.text)
				  return false;

			  if ((comment() != 0) && (other.comment() != 0) &&
				      *(comment()) != *(other.comment()))
				  return false;

			  if (((comment() == 0) && (other.comment() != 0)) ||
			      ((comment() != 0) && (other.comment() == 0)))
				  return false;

			  return true;
		}
		
		// 
		// get from and to squares from ply text.
		// complete_from = true: you ask for the full square text of the ply 
		// (i.e. 'f3' if Nf3-e5 has been played)
		// complete_from = false: you ask for what you need to write down the
		// ply. So you can get '' (ply=Ne5) or 'f' (Nfe5) or '3' (N3e5).
		// 
		void squares(std::string &from, std::string &to, bool complete_from);

		bool promotion() const 
		{
			return text.find("=") != std::string::npos;
		}

		Piece fromPiece() const;
		Piece toPiece() const;
		
		bool isLongCastle() const;
		bool isShortCastle() const;

		virtual bool supportingComments() const = 0;
		virtual bool supportingVariations() const = 0;
		virtual void bindComment(const CommentText &comment) = 0;
		virtual void unbindComment() = 0;
		virtual void bindVariation(const MoveList &variation) = 0;
		virtual void unbindVariations() = 0;
		virtual pgn::CommentText const* comment() const = 0;
		virtual std::vector<pgn::MoveList *> const* variations() const = 0;
	};

	struct PlyDataNaked : public PlyData
	{
		PlyDataNaked()
		:PlyData()
		{}

		~PlyDataNaked()
		{}

		PlyData* clone() const 
		{ 
			PlyDataNaked *p = new PlyDataNaked(); 
			p->text = this->text;
			p->fromsquare_letter = this->fromsquare_letter;
			p->fromsquare_number = this->fromsquare_number;
			p->piece = this->piece;
			return p;
		}
		
		bool supportingComments() const { return false; }
		bool supportingVariations() const { return false; }
		void bindComment(const CommentText &comment) {} // nop
		void unbindComment() {} // nop
		void bindVariation(const MoveList &variation) {} // nop
		void unbindVariations() {} // nop
		pgn::CommentText const* comment() const { return 0; }
		std::vector<pgn::MoveList *> const* variations() const { return 0; }
	};

	struct PlyDataComment : public PlyData
	{
		pgn::CommentText *comment_;
			
		PlyDataComment()
		:PlyData(),comment_(0)
		{}

		~PlyDataComment()
		{
			unbindComment();
		}

		PlyData* clone() const 	
		{ 
			PlyDataComment *p = new PlyDataComment(); 
			p->text = this->text;
			p->fromsquare_letter = this->fromsquare_letter;
			p->fromsquare_number = this->fromsquare_number;
			p->piece = this->piece;
			if (this->comment_)
				p->comment_ = new pgn::CommentText(*(this->comment_));
			else
				p->comment_ = 0;
			return p;
		}

		bool supportingComments() const { return true; }
		bool supportingVariations() const { return false; }
		void bindComment(const CommentText &comment) 
		{
			delete this->comment_;
			this->comment_ = new pgn::CommentText(comment);
		}
		void unbindComment() 
		{
			delete comment_; comment_ = 0;
		}
		void bindVariation(const MoveList &variation) {} // nop
		void unbindVariations() {} // nop
		pgn::CommentText const* comment() const { return comment_; }
		std::vector<pgn::MoveList *> const* variations() const { return 0; }
	};

	struct PlyDataVariation : public PlyData
	{
		std::vector<pgn::MoveList *> variation_;
			
		PlyDataVariation()
		:PlyData()
		{}

		~PlyDataVariation()
		{
			unbindVariations();
		}

		PlyData* clone() const 	
		{ 
			PlyDataVariation *p = new PlyDataVariation(); 
			p->text = this->text;
			p->fromsquare_letter = this->fromsquare_letter;
			p->fromsquare_number = this->fromsquare_number;
			p->piece = this->piece;
			for (unsigned int i=0; i<this->variation_.size(); ++i)
				p->variation_.push_back(new pgn::MoveList(*(this->variation_[i])));
			return p;
		}

		bool supportingComments() const { return false; }
		bool supportingVariations() const { return true; }
		void bindComment(const CommentText &comment) {} // nop
		void unbindComment() {} // nop
		void bindVariation(const MoveList &variation) 
		{
			this->variation_.push_back(new pgn::MoveList(variation));		  
		} 
		void unbindVariations() 
		{
			for (unsigned int i=0; i<variation_.size(); ++i)
				 delete variation_[i];
			variation_.clear();
		}
		pgn::CommentText const* comment() const { return 0; }
		std::vector<pgn::MoveList *> const* variations() const { return &variation_; }
	};

	struct PlyDataFull : public PlyData
	{
		pgn::CommentText *comment_;
		std::vector<pgn::MoveList *>variation_;
			
		PlyDataFull()
		:PlyData(),comment_(0)
		{}

		~PlyDataFull()
		{
			unbindComment();
			unbindVariations();
		}

		PlyData* clone() const 	
		{ 
			PlyDataFull *p = new PlyDataFull(); 
			p->text = this->text;
			p->fromsquare_letter = this->fromsquare_letter;
			p->fromsquare_number = this->fromsquare_number;
			p->piece = this->piece;
			if (this->comment_)
				p->comment_ = new pgn::CommentText(*(this->comment_));
			else
				p->comment_ = 0;
			for (unsigned int i=0; i<this->variation_.size(); ++i)
				p->variation_.push_back(new pgn::MoveList(*(this->variation_[i])));
			return p;
		}

		bool supportingComments() const { return true; }
		bool supportingVariations() const { return true; }
		void bindComment(const CommentText &comment) 
		{
			delete this->comment_;
			this->comment_ = new pgn::CommentText(comment);
		}
		void unbindComment() 
		{
			delete comment_; comment_ = 0;
		}
		void bindVariation(const MoveList &variation) 
		{
			this->variation_.push_back(new pgn::MoveList(variation));		  
		} 
		void unbindVariations() 
		{
			for (unsigned int i=0; i<variation_.size(); ++i)
				 delete variation_[i];
			variation_.clear();
		}
		pgn::CommentText const* comment() const { return comment_; }
		std::vector<pgn::MoveList *> const* variations() const { return &variation_; }
	};

	void PlyData::squares(std::string &from, std::string &to, bool complete_from)
	{
		char x_coord = 0;
		char y_coord = 0;
		char e_coord = 0;
		for (unsigned int i=0; i<text.size(); ++i)
		{
			if ((text[i] >= 'a' && text[i] <= 'h') ||
				(text[i] >= '1' && text[i] <= '8')) 
			{
				if (!e_coord)
					e_coord = text[i];
				else if (!x_coord)
					x_coord = text[i];
				else
					y_coord = text[i];
			}
		}		
		if (!y_coord)
		{
			y_coord = x_coord;
			x_coord = e_coord;
			e_coord = 0;
		}
		std::stringstream ss;
		// I am the public Ply::fromSquare() method: I need the full info
		if ((complete_from == true) && fromsquare_letter && fromsquare_number)
		{
  	  		ss << fromsquare_letter << fromsquare_number;
		}
		// I am the operator << (): I need the minimum info to disambiguate the move
		else if (e_coord)
		{
  			ss << e_coord;
		}
			
		from = ss.str();
		ss.str("");
		ss <<  x_coord << y_coord;
		to = ss.str();
	}
	
	Piece PlyData::fromPiece() const
	{
		if (isLongCastle() || isShortCastle())
			return pgn::Piece::King();
			
		switch (text[0])
		{
			case 'R':
				return pgn::Piece::Rook();
			case 'K':
				return pgn::Piece::King();
			case 'Q':
				return pgn::Piece::Queen();
			case 'B':
				return pgn::Piece::Bishop();
			case 'N':
				return pgn::Piece::Knight();
			default:
				return pgn::Piece::Pawn();
		}
	}
	
	Piece PlyData::toPiece() const
	{
		if (promotion())
		{
			size_t pos = text.find("=");
			return pgn::Piece(text[pos+1]); 
		}
		else
			return fromPiece();
	}

};

pgn::Ply::Ply(const pgn::Ply& src) 
{
	hdata = src.hdata->clone();
}

pgn::Ply::Ply()
{
	hdata = new PlyDataNaked;
}

pgn::Ply::Ply(const std::string& ply_text)
{
	hdata = new PlyDataNaked;
	hdata->text = ply_text;
	hdata->fromsquare_letter = 0;
	hdata->fromsquare_number = 0;
	hdata->piece = hdata->fromPiece();
}

pgn::Ply::~Ply() 
{
	delete hdata; hdata=0;
}

pgn::Ply& pgn::Ply::operator = (const pgn::Ply& src) 
{
	if (&src == this)
		return(*this);

	delete hdata;
	hdata = src.hdata->clone();

	return *this;
}

std::string pgn::Ply::str() const
{
	return hdata->text;
}

bool pgn::Ply::valid() const
{
	return isShortCastle() || isLongCastle() || toSquare().valid();
}

bool pgn::Ply::operator == (const pgn::Ply& src) const
{
	return *hdata == *(src.hdata);
}

std::ostream& pgn::operator << ( std::ostream& os, const pgn::Ply& src ) 
{
	if (!src.valid())
	{
		return os;
	}
	
	if (src.isLongCastle())
	{
		os << "O-O-O";
		if (src.isCheckMate())
			os << "#";
		else if (src.isCheck())
			os << "+";
	}
	else if (src.isShortCastle())
	{
		os << "O-O";
		if (src.isCheckMate())
			os << "#";
		else if (src.isCheck())
			os << "+";
	}
	else
	{
		os << src.piece();
		std::string from, to;
		src.hdata->squares(from, to, false);
		if (!from.empty())
			os << pgn::Square(from[0], from[1]);
		if (src.isCapture())
			os << "x";
		os << src.toSquare();
		if (src.promotion())
			os << "=" << src.promoted();
		if (src.isCheckMate())
			os << "#";
		else if (src.isCheck())
			os << "+";
	}

	if (src.glyphValue() >= 0)
	{
		os << "$" << src.glyphValue();
	}
	
	if (src.hdata->comment())
		os << *(src.hdata->comment());

	if (src.hdata->variations())
		for (unsigned int i=0; i< src.hdata->variations()->size(); ++i)
			  os << "(" << *(*(src.hdata->variations()))[i] << ") ";

	return os;
}

std::istream& pgn::operator >> ( std::istream& is, pgn::Ply& src ) 
{
	std::string str;
	std::copy(std::istreambuf_iterator<char>(is), std::istreambuf_iterator<char>(), std::inserter(str, str.end()));
	pgn::Parser parser;
	std::string::const_iterator itr1 = str.begin();
	std::string::const_iterator itr2 = str.end();
	parser.getPly(itr1, itr2, src);
	return is;
}

bool pgn::Ply::operator != (const pgn::Ply& src) const 
{
	return !(src == *this);
}

bool pgn::PlyData::isLongCastle() const 
{
	if (text.find("O-O-O") != std::string::npos)
		return true;
	else
		return false;
}

bool pgn::PlyData::isShortCastle() const 
{
  	if ((text.find("O-O") != std::string::npos) && !isLongCastle())
		return true;
	else
		return false;
}

bool pgn::Ply::isLongCastle() const 
{
	return (hdata->isLongCastle());
}

bool pgn::Ply::isShortCastle() const 
{
	return (hdata->isShortCastle());
}

bool pgn::Ply::isCapture() const 
{
	if (hdata->text.find("x") != std::string::npos)
		return true;
	else
		return false;    
}

bool pgn::Ply::isCheck() const 
{ 
	if (hdata->text.find("+") != std::string::npos)
		return true;
	else
		return false;    
}

bool pgn::Ply::isCheckMate() const 
{ 
	if (hdata->text.find("#") != std::string::npos)
		return true;
	else
		return false;    
}

pgn::Square pgn::Ply::fromSquare() const 
{
	std::string from;
	std::string to;
	hdata->squares(from, to, true);

	if (from.empty()) 
		return pgn::Square(0, 0, hdata->fromPiece());
	else if ((from.size() == 1) && (from[0] >= '1') && (from[0] <= '8'))
		return pgn::Square(0, from[0], hdata->fromPiece());
	else if ((from.size() == 1) && (from[0] >= 'a') && (from[0] <= 'h'))
	  	return pgn::Square(from[0], 0, hdata->fromPiece());
	else
		return pgn::Square(from[0], from[1], hdata->fromPiece());
}

pgn::Square pgn::Ply::toSquare() const 
{
	std::string from;
	std::string to;
	hdata->squares(from, to, true);
	return pgn::Square(to[0], to[1], hdata->toPiece());
}

pgn::Piece pgn::Ply::piece() const 
{
	return hdata->piece;
}

bool pgn::Ply::promotion() const 
{
	return hdata->promotion();
}

pgn::Piece pgn::Ply::promoted() const 
{
	if (hdata->promotion())
		return toSquare().piece();
	else
		return pgn::Piece::Null();
}

void pgn::Ply::setGlyphValue(int glyphval)
{
	size_t pos = hdata->text.find("$");
      
	if (pos != std::string::npos)
		hdata->text = hdata->text.substr(0, pos);
	
	std::stringstream ss;
	ss << hdata->text << "$" << glyphval;
	hdata->text = ss.str();
}

void pgn::Ply::setFromSquare(const Square &s)
{
	hdata->fromsquare_letter = s.col();
	hdata->fromsquare_number = s.row();
}

void pgn::Ply::setToSquare(const Square &s)
{
    // For the time being the 'to' coordinates are coded directly into hdata->text (that's going to change soon)
    int colpos = hdata->text.find_last_of("abcdefgh");
    int rowpos = colpos + 1;
    hdata->text[colpos] = s.col();
    hdata->text[rowpos] = s.row();
}

void pgn::Ply::setPiece(const Piece &p)
{
	hdata->piece = p;
}

int pgn::Ply::glyphValue() const
{
	size_t pos = hdata->text.find("$");
	
	if (pos == std::string::npos)
		return -1;

	return atoi(hdata->text.substr(pos+1).c_str());
}

void pgn::Ply::bindComment(const CommentText &comment)
{
	if (!hdata->supportingComments() && hdata->supportingVariations())
	{
		delete hdata;
		hdata = new PlyDataFull();
	}
	else if (!hdata->supportingComments() && !hdata->supportingVariations())
	{
  		delete hdata;
		hdata = new PlyDataComment();
	}
	hdata->bindComment(comment);
}

void pgn::Ply::unbindComment()
{
  	if (hdata->supportingComments())
	{
		  hdata->unbindComment();
	}
}

void pgn::Ply::bindVariation(const MoveList &variation)
{
  	if (!hdata->supportingVariations() && hdata->supportingComments())
	{
  		delete hdata;
		hdata = new PlyDataVariation();
	}
	else if (!hdata->supportingVariations() && !hdata->supportingComments())
	{
		delete hdata;
		hdata = new PlyDataFull();
	}
	hdata->bindVariation(variation);
}

void pgn::Ply::unbindVariations()
{
  	if (hdata->supportingVariations())
	{
	  	hdata->unbindVariations();
	}
}
