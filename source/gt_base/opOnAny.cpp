/*
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

#include "opOnAny.hpp"

////////////////////////////////////////////////////////////
using namespace gt;

void
gt::voidAssign::textToNStr(const dText *pFrom, void *pTo){
	*reinterpret_cast<dNatStr*>(pTo) = ::toNStr(*pFrom);
}

void
gt::voidAssign::textToPStr(const dText *pFrom, void *pTo){
	*reinterpret_cast<dStr*>(pTo) = ::toPStr(*pFrom);
}

void
gt::voidAssign::plaCStrToPStr(const dPlaChar * const *pFrom, void *pTo){
	*reinterpret_cast<dStr*>(pTo) = *pFrom;
}

void
gt::voidAssign::plaCStrToNStr(const dPlaChar * const *pFrom, void *pTo){
	*reinterpret_cast<dNatStr*>(pTo) = ::toNStr(*pFrom);
}

void
gt::voidAssign::plaCStrToText(const dPlaChar * const *pFrom, void *pTo){
	*reinterpret_cast<dText*>(pTo) = ::toText(*pFrom);
}

void
gt::voidAppend::textToText(const dText *pFrom, void *pTo){
	reinterpret_cast<
		std::basic_string<dTextChar, std::char_traits<dTextChar> >*
	>(pTo)->append(*pFrom);
}

void
gt::voidAppend::plaCStrToPStr(const dPlaChar * const *pFrom, void *pTo){
	reinterpret_cast<dStr*>(pTo)->append(*pFrom);
}

void
gt::voidAppend::plaCStrToNStr(const dPlaChar * const *pFrom, void *pTo){
	reinterpret_cast<
		std::basic_string<dNatChar, std::char_traits<dNatChar> >*
	>(pTo)->append( ::toNStr(*pFrom) );
}

void
gt::voidAppend::plaCStrToText(const dPlaChar * const *pFrom, void *pTo){
	reinterpret_cast<
		std::basic_string<dTextChar, std::char_traits<dTextChar> >*
	>(pTo)->append( ::toText(*pFrom) );
}


