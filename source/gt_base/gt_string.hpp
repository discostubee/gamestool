/*
 * !\file	string.hpp
 * !\brief	These tools aim to support as many of the different string formats as possible.
 * !\note	This section does not have to be platform neutral.
 */
#ifndef STRING_HPP
#define STRING_HPP

#include <stdio.h>
#include <string.h>
#include <string>

//--------------------------------------------------------
typedef char dNatChar;	//!< char type native to gamestools.

//--------------------------------------------------------
//!\brief	stl string wrapper of the native string type.
//!\note	Carl Daniel http://social.msdn.microsoft.com/forums/en-US/vcgeneral/thread/d752f215-e89f-4c87-acdc-7cf6e9e0e705
typedef std::basic_string<dNatChar, std::char_traits<dNatChar> > dStr;

//--------------------------------------------------------
//!\brief
size_t natCharLen(const dNatChar* pString);

//--------------------------------------------------------
//!\brief	Converts the source and modifies the pre-allocated destination.
void PStrToNStr(const char *source, dNatChar *destination, size_t length);


#endif
