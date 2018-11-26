//
// Copyright (C) 2002 Andrea Vinzoni <vinzo@planet.it>
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

#include "PGNException.h"

pgn::invalid_castle_string::invalid_castle_string() throw()
:pgn::parse_exception("invalid string for castle (maybe 0 (zero) used instead of 'O')")
{
}

pgn::invalid_ply_text::invalid_ply_text(const std::string &ply) throw()
:pgn::parse_exception("invalid text in ply >" + ply + "<")
{
}

pgn::invalid_tag::invalid_tag(const std::string &tag) throw()
:pgn::parse_exception("invalid text in tag >" + tag + "<")
{
}

pgn::invalid_result::invalid_result() throw()
:pgn::parse_exception("invalid result")
{
}

pgn::missing_result::missing_result() throw()
:pgn::parse_exception("missing result")
{
}

pgn::unsupported_variations::unsupported_variations() throw()
:pgn::parse_exception("support for variations not yet available in pgnlib")
{
}

pgn::unsupported_glyphs::unsupported_glyphs() throw()
:pgn::parse_exception("support for glyphs not yet available in pgnlib")
{
}

pgn::invalid_fen_string::invalid_fen_string(const std::string &fenstring) throw()
:message_("invalid characters in fen string >" + fenstring + "<")
{
}