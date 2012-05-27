/*
 * !\file		string.hpp
 * !\brief
 * !\note		Text is a string type that is meant to be displayed. Any other string type is only meant for internal logging or for coding.
 * !\note		Carl Daniel http://social.msdn.microsoft.com/forums/en-US/vcgeneral/thread/d752f215-e89f-4c87-acdc-7cf6e9e0e705
 *
**********************************************************************************************************
 *  Copyright (C) 2010  Stuart Bridgens
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License (version 3) as published by
 *  the Free Software Foundation.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *********************************************************************************************************
 */
#ifndef STRING_HPP
#define STRING_HPP

#include <stdio.h>
#include <string.h>	//- C library.
#include <string>	//- c++ library.

#ifdef DEBUG
	#include "unitTests.hpp"
#endif

//--------------------------------------------------------

typedef char dNatChar;		//!< char type native to gamestools, which is 8bit ascii across all platforms.
typedef char dPlaChar;		//!< char type most efficient (speed wise) for the target platform.
typedef wchar_t dTextChar;	//!< Used for display text.

//!\brief	stl string wrapper of the native string type.
typedef std::basic_string<dNatChar, std::char_traits<dNatChar> > dStr;

typedef std::basic_string<dPlaChar, std::char_traits<dPlaChar> > dPlaStr;

//!\brief	Text strings are used for displaying text. Intended to be UTF16 allowing for compression and multiple languages.
typedef std::basic_string<dTextChar, std::char_traits<dTextChar> > dText;

//!\brief	Find length of a native string
size_t natCStrLen(const dNatChar *pString);

//!\brief	Find length of a platform string.
size_t plaCStrLen(const dPlaChar *pString);

//!\brief	Converts a native C style string to a text string instance.
dText NCStrToText(const dNatChar *pString);

//!\brief	Converts platform C style string to native C++ string instance.
dStr PCStrToNStr(const dPlaChar *pString);

//!\brief	Converts platform C style string to text string instance.
dText PCStrToText(const dPlaChar *pString);

//!\brief	Convert from a text C style string to a native string instance.
dStr textToNStr(const dTextChar *pString);


#endif
