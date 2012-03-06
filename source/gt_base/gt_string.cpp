#include "gt_string.hpp"


size_t 
natCharLen(const dNatChar* pString)
{
	return ::strlen(pString);
}

size_t plaCharLen(const dPlaChar *pString){
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
