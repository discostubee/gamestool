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
#include "exceptions.hpp"

#ifdef USE_BOOST_LOCALE
#	define USE_LOCALE_GEN
#endif

#if defined(USE_BOOST_LOCALE) || defined(USE_LOCALE_GEN)
	static const char *nativeEncoding="ISO-8859-1";
	static const char *textEncoding="UTF-8";
#endif

//--------------------------------------------------------

#ifdef USE_LOCALE_GEN
static std::locale&
getPlatformEncode(){
	static bool setup = false;
	static std::locale loc;
	if(!setup){
		boost::locale::generator gen;
		loc = gen.generate("");	//- Default system encoding
		std::locale::global(loc);
		setup=true;
	}

	return loc;
}

static std::locale&
getNativeEncode(){
	static bool setup = false;
	static std::locale loc;
	if(!setup){
		boost::locale::generator gen;
		gen.use_ansi_encoding(true);
		loc = gen.generate(nativeEncoding);

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
		loc = gen.generate(textEncoding);
		setup=true;
	}

	return loc;
}
#endif

//--------------------------------------------------------

size_t
PCStrLen(const dPlaChar *pString){
	return ::strlen(pString);
}

dNatStr
PCStr2NStr(const dPlaChar *pString){
	dNatStr strRtn;

	if(pString == NULL)
		return strRtn;

#	if defined(USE_LOCALE_GEN)
		strRtn = boost::locale::conv::to_utf<dNatChar>(
			pString,
			&pString[PCStrLen(pString)],
			nativeEncoding,
			boost::locale::conv::skip
		);
#	elif defined(USE_BOOST_LOCALE)
		strRtn = boost::locale::conv::to_utf<dNatChar>(
			pString,
			&pString[PCStrLen(pString)],
			nativeEncoding
		);
#	else
		strRtn = pString;	//!!! temporary measure.
#	endif


	return strRtn;
}

dText
PCStr2Text(const dPlaChar *pString){
	dText strRtn;

	if(pString == NULL)
		return strRtn;

#	if defined(USE_LOCALE_GEN)
		strRtn = boost::locale::conv::to_utf<dTextChar>(pString, &pString[PCStrLen(pString)], getTextEncode());
#	elif defined(USE_BOOST_LOCALE)
		strRtn = boost::locale::conv::to_utf<dTextChar>(pString, &pString[PCStrLen(pString)], textEncoding);
#	else
		strRtn.t = pString;	//!!! temporary measure.
#	endif

	return strRtn;
}


size_t
NCStrLen(const dNatChar *pString){
	if(pString == NULL)
		return 0;

	return strlen(pString);
}

dStr
NCStr2PStr(const dNatChar *pString){
	dStr strRtn;

	if(pString == NULL)
		return strRtn;

#	if defined(USE_LOCALE_GEN)
		strRtn = boost::locale::conv::to_utf<dNatChar>(pString, &pString[NCStrLen(pString)], getPlatformEncode());
#	elif defined(USE_BOOST_LOCALE)
		strRtn = boost::locale::conv::to_utf<dNatChar>(pString, &pString[NCStrLen(pString)], "UTF-8");
#	else
		strRtn = pString;	//!!! temporary measure.
#	endif

	return strRtn;
}

dText
NCStr2Text(const dNatChar *pString){
	dText strRtn;

	if(pString == NULL)
		return strRtn;

#	if defined(USE_LOCALE_GEN)
		strRtn = boost::locale::conv::to_utf<dTextChar>(pString, &pString[NCStrLen(pString)], getTextEncode());
#	elif defined(USE_BOOST_LOCALE)
		strRtn = boost::locale::conv::to_utf<dTextChar>(pString, &pString[NCStrLen(pString)], textEncoding);
#	else
		strRtn = pString;	//!!! temporary measure.
#	endif

	return strRtn;
}


dStr
toPStr(const dNatStr &pString){
	return NCStr2PStr(pString.t.c_str());
}

dText
toText(const dNatStr &pString){
	return NCStr2Text(pString.t.c_str());
}

dNatStr
toNStr(const dStr &pString){
	return PCStr2NStr(pString.c_str());
}

dText
toText(const dStr &pString){
	return PCStr2Text(pString.c_str());
}



dStr
toPStr(const dText &pString){
	dStr strRtn;

	if(pString.t.empty())
		return strRtn;

	#if defined(USE_LOCALE_GEN)
		strRtn = boost::locale::conv::to_utf<dPlaChar>(pString.t, getPlatformEncode());
	#elif defined(USE_BOOST_LOCALE)
		strRtn = boost::locale::conv::to_utf<dPlaChar>(pString.t, "UTF-8");
	#else
		strRtn = pString.t;	//!!! temporary measure.
	#endif


	return strRtn;
}

dNatStr
toNStr(const dText &pString){
	dNatStr strRtn;

	if(pString.t.empty())
		return strRtn;

#	if defined(USE_LOCALE_GEN)
		strRtn = boost::locale::conv::to_utf<dNatChar>(pString.t, getNativeEncode());
#	elif defined(USE_BOOST_LOCALE)
		strRtn = boost::locale::conv::to_utf<dNatChar>(pString.t, nativeEncoding, boost::locale::conv::skip);
#	else
		strRtn = pString.t;	//!!! temporary measure.
#	endif

	return strRtn;
}

bool
hasWildStr(const char *findIn, const char *wildStr){
	static const unsigned int NOWILD = static_cast<unsigned int>(-1);

	if(findIn == NULL || wildStr == NULL)
		return false;

	const unsigned int lenA = strlen(findIn);
	const unsigned int lenB = strlen(wildStr);

	if(lenA == 0 || lenB == 0)
		return false;

	unsigned int idxBWildStart = NOWILD, idxBWildEnd = NOWILD;
	unsigned int idxA=0, idxB=0;

	while(idxA < lenA && idxB < lenB){
		if(wildStr[idxB] == '*' && idxBWildStart == NOWILD){
			if(idxB +1 == lenB)
				return true;

			++idxB;

			if(wildStr[idxB] != '*')
				idxBWildStart = idxB;

		}else if(idxBWildStart != NOWILD && idxBWildEnd == NOWILD){
			if(wildStr[idxB] == '*')
				idxBWildEnd = idxB;
			else if(idxB +1 >= lenB)
				idxBWildEnd = idxB +1;
			else
				++idxB;

		}else if(idxBWildStart != NOWILD && idxBWildEnd != NOWILD){
			const unsigned int searchLen = idxBWildEnd -idxBWildStart;

			unsigned int idxWild, idxFind;
			for(idxFind = idxA; idxFind < lenA -searchLen +1; ++idxFind){
				for(idxWild = 0; idxWild < searchLen; ++idxWild){
					if(findIn[idxFind +idxWild] != wildStr[idxBWildStart +idxWild])
						break;
				}

				if(idxWild == searchLen)
					break;
			}

			if(idxFind == lenA -searchLen +1)
				return false;

			idxA += searchLen;
			idxB += searchLen;
			idxBWildStart = idxBWildEnd = NOWILD;

		}else if(findIn[idxA] != wildStr[idxB]){
			return false;

		}else{
			++idxA;
			++idxB;
		}
	}

	return true;	//- made it all the way.
}

#ifdef GTUT
#include "unitTests.hpp"

GTUT_START(test_string, wildStr){
	const char * testA = "dogcat";
	const char * wildA = "*cat";
	const char * wildB = "dog*";
	const char * wildC = "*gc*";
	const char * wildD = "d*t";
	const char * notwildA = "not*";
	const char * notwildB = "*not";
	const char * notwildC = "*not*";
	const char * notwildD = "d*not";

	GTUT_ASRT(hasWildStr(testA, wildA), "Failed to find wild A");
	GTUT_ASRT(hasWildStr(testA, wildB), "Failed to find wild B");
	GTUT_ASRT(hasWildStr(testA, wildC), "Failed to find wild C");
	GTUT_ASRT(hasWildStr(testA, wildD), "Failed to find wild D");
	GTUT_ASRT(!hasWildStr(testA, notwildA), "false positive A");
	GTUT_ASRT(!hasWildStr(testA, notwildB), "false positive B");
	GTUT_ASRT(!hasWildStr(testA, notwildC), "false positive C");
	GTUT_ASRT(!hasWildStr(testA, notwildD), "false positive D");

}GTUT_END;

#endif
