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
#include <string> //- C++ lib.
#include <string.h>	//- C lib, for strlen
#include <boost/version.hpp>
#include <boost/serialization/strong_typedef.hpp>
#if (BOOST_VERSION / 100 % 1000) >= 49
#	define USE_BOOST_LOCALE
#	include <boost/locale.hpp>
#endif
//--------------------------------------------------------

typedef char dPlaChar;		//!< Raw buffer type most efficient (speed wise) for the target platform to use internally. Typically this is UTF-8. Note that the number of bytes isn't the number of characters.
typedef char dNatChar;		//!< char type native to gamestools, which is 8bit ascii across all platforms.
typedef char dTextChar;		//!< Used for UTF-8 text which will be displayed to the user. Note: The buffer size isn't linked just to the number of characters.

typedef std::basic_string<dPlaChar, std::char_traits<dPlaChar> > dStr;	//!< stl string wrapper of the platform string type. Calling it dStr and not dPlaStr because I'm lazy and this is the most common string type. Not using boost strong typedef here so that string streams and string literals work with it.
typedef std::basic_string<dNatChar, std::char_traits<dNatChar> > dNatStr_def;	//!< stl string wrapper of the string type native to gamestool.
typedef std::basic_string<dTextChar, std::char_traits<dTextChar> > dText_def;	//!< Text strings are used for displaying text.

BOOST_STRONG_TYPEDEF(dNatStr_def, dNatStr)
BOOST_STRONG_TYPEDEF(dText_def, dText)

//--------------------------------------------------------

size_t PCStrLen(const dPlaChar *pString);	//!< Find length of a platform string. Only platform strings offer raw buffer support, because string literals are all platform based.
dNatStr PCStr2NStr(const dPlaChar *pString);	//!< Converts platform C style string to native C++ string instance.
dText PCStr2Text(const dPlaChar *pString);	//!< Converts platform C style string to text string instance.

size_t NCStrLen(const dNatChar *pString);
dStr NCStr2PStr(const dNatChar *pString);
dText NCStr2Text(const dNatChar *pString);

dStr toPStr(const dNatStr &pString);
dText toText(const dNatStr &pString);	//!< Converts a native C style string to a text string instance.

dNatStr toNStr(const dStr &pString);
dText toText(const dStr &pString);

dNatStr toNStr(const dText &pString);	//!< Convert from a text C style string to a native string instance.
dStr toPStr(const dText &pString);	//!< Convert from a text C style string to a platform string instance.


#endif
