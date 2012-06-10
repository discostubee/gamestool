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


//--------------------------------------------------------


static std::locale&
getNativeEncode(){
	static bool setup = false;
	static std::locale loc;
	if(!setup){
		boost::locale::generator gen;
		gen.use_ansi_encoding(true);
		loc = gen.generate("en_US.UTF-8");
		setup=true;
	}

	return loc;
}

//!\brief	Modify this to be whatever the platform is encoding with.
static std::locale&
getPlatformEncode(){
	static bool setup = false;
	static std::locale loc;
	if(!setup){
		boost::locale::generator gen;
		loc = gen.generate("");
		setup=true;
	}

	return loc;
}

static std::locale&
getTextEncode(){
	static bool setup = false;
	static std::locale loc;
	if(!setup){
		boost::locale::generator gen;
		loc = gen.generate("en_US.UTF-8");
		setup=true;
	}

	return loc;
}

//--------------------------------------------------------

dStr
toPStr(const dNatStr &pString){
	dStr strRtn;

	if(pString.t.empty())
		return strRtn;

	strRtn = boost::locale::conv::to_utf<dPlaChar>(pString.t, getPlatformEncode());

	return strRtn;
}

dText
toText(const dNatStr &pString){
	dText strRtn;

	if(pString.t.empty())
		return strRtn;

	strRtn = boost::locale::conv::to_utf<dTextChar>(pString.t, getTextEncode());

	return strRtn;
}

size_t
PCStrLen(const dPlaChar *pString){
	return ::strlen(pString);
}

dNatStr
toNStr(const dPlaChar *pString){
	dNatStr strRtn;

	if(pString == NULL)
		return strRtn;

	size_t inLen = PCStrLen(pString);

	if(inLen==0)
		return strRtn;

	strRtn = boost::locale::conv::to_utf<dNatChar>(pString, &pString[inLen-1], getNativeEncode());

	return strRtn;
}

dStr
toPStr(const dText &pString){
	dStr strRtn;

	if(pString.t.empty())
		return strRtn;

	strRtn = boost::locale::conv::to_utf<dPlaChar>(pString.t, getPlatformEncode());

	return strRtn;
}

dText
toText(const dPlaChar *pString){
	dText strRtn;

	if(pString == NULL)
		return strRtn;

	size_t inLen = PCStrLen(pString);

	if(inLen==0)
		return strRtn;

	strRtn = boost::locale::conv::to_utf<dTextChar>(pString, &pString[inLen-1], getTextEncode());

	return strRtn;
}

dNatStr
toNStr(const dText &pString){
	dNatStr strRtn;

	if(pString.t.empty())
		return strRtn;

	strRtn = boost::locale::conv::to_utf<dNatChar>(pString.t, getNativeEncode());

	return strRtn;
}






