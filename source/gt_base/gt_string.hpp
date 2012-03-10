/*
 * !\file		string.hpp
 * !\brief
 * !\note		Text is a string type that is meant to be displayed. Any other string type is only meant for internal logging or for coding.
 * !\note		Carl Daniel http://social.msdn.microsoft.com/forums/en-US/vcgeneral/thread/d752f215-e89f-4c87-acdc-7cf6e9e0e705
 */
#ifndef STRING_HPP
#define STRING_HPP

#include <stdio.h>
#include <string.h>	//- C library.
#include <string>		//- c++ library.

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

//--------------------------------------------------------
//- Functions defined in this library.

//!\brief
size_t natCharLen(const dNatChar *pString);

size_t plaCharLen(const dPlaChar *pString);

//--------------------------------------------------------
//- Functions defined in other libraries.

//!\brief	Converts a native string to a text string instance.
dText NCStrToText(const dNatChar *pString);

dStr PCStrToNStr(const dPlaChar *pString);


#endif
