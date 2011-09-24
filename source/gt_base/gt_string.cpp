#include "gt_string.hpp"


size_t 
natCharLen(const dNatChar* pString)
{
	return ::strlen(pString);
}

void PStrToNStr(const char *source, dNatChar *destination, size_t length )
{
	//!\todo
	::strncpy(destination, source, length);
}
