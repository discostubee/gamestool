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

#include "gt_string.hpp"


size_t 
natCStrLen(const dNatChar* pString)
{
	return ::strlen(pString);
}

size_t plaCStrLen(const dPlaChar *pString){
	return ::strlen(pString);
}

dText NCStrToText(const dNatChar *pString){
	dText strRtn;

	//!\todo

	return strRtn;
}

dStr PCStrToNStr(const dPlaChar *pString){
	dStr strRtn;

	//!\todo
	strRtn.assign(pString);

	return strRtn;
}

dText PCStrToText(const dPlaChar *pString){
	dText strRtn;

	return strRtn;
}
