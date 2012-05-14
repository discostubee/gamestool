
#include "binPacker.hpp"

void
bpk::pack(const dStr *packMe, dByte **output, size_t *sizeOut){
	ASRT_NOTNULL(packMe); ASRT_NOTNULL(output);

	(*sizeOut) = (packMe->length() + 1) * sizeof(dNatChar) + sizeof(size_t);
	size_t idxTerm = (*sizeOut)-sizeof(dNatChar);
	if(*sizeOut == 0)
		return;

	delete(*output);
	(*output) = new dByte[*sizeOut];
	*reinterpret_cast<size_t*>(*output) = packMe->length();
	if(!packMe->empty()){
		memcpy(
			(*output)+sizeof(size_t),
			packMe->data(),
			idxTerm -sizeof(size_t)
		);
	}
	(*output)[idxTerm] = '\0';
}

void
bpk::unpack(const dByte *unpackFrom, dStr *unpackTo, size_t *sizeOut, size_t limit){
	ASRT_NOTNULL(unpackFrom); ASRT_NOTNULL(unpackTo); ASRT_NOTNULL(sizeOut);

	size_t numChars=0;
	memcpy(&numChars, unpackFrom, sizeof(size_t));
	unpackTo->assign( unpackFrom+sizeof(size_t), numChars);
}

void
bpk::pack(const dText *packMe, dByte **output, size_t *sizeOut){
	ASRT_NOTNULL(packMe); ASRT_NOTNULL(output);

	(*sizeOut) = (packMe->length() + 1) * sizeof(dTextChar) + sizeof(size_t);
	size_t idxTerm = (*sizeOut)-sizeof(dTextChar);
	if(*sizeOut == 0)
		return;

	delete(*output);
	(*output) = new dByte[*sizeOut];
	*reinterpret_cast<size_t*>(*output) = packMe->length();
	if(!packMe->empty()){
		memcpy(
			(*output)+sizeof(size_t),
			packMe->data(),
			idxTerm -sizeof(size_t)
		);
	}
	(*output)[idxTerm] = L'\0';
}

void
bpk::unpack(const dByte *unpackFrom, dText *unpackTo, size_t *sizeOut, size_t limit){
	ASRT_NOTNULL(unpackFrom); ASRT_NOTNULL(unpackTo); ASRT_NOTNULL(sizeOut);

	size_t numChars=0;
	memcpy(&numChars, unpackFrom, sizeof(size_t));
	unpackTo->assign( reinterpret_cast<const dTextChar*>(unpackFrom+sizeof(size_t)), numChars);
}

////////////////////////////////////////////////////////////
// Tests
#ifdef GTUT



#endif

