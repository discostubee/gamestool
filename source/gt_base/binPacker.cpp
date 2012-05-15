
#include "binPacker.hpp"

void
bpk::pack(const dStr *packMe, dByte **output, size_t *sizeOut){
	ASRT_NOTNULL(packMe); ASRT_NOTNULL(output);

	if(packMe->empty()){
		(*sizeOut) = sizeof(size_t);
	}else{
		(*sizeOut) = (packMe->length() + 1) * sizeof(dNatChar) + sizeof(size_t);
	}


	delete(*output);
	(*output) = new dByte[*sizeOut];
	*reinterpret_cast<size_t*>(*output) = packMe->length();

	if(!packMe->empty()){
		memcpy(
			(*output)+sizeof(size_t),
			packMe->data(),
			(*sizeOut) - sizeof(size_t)
		);
	}
}

void
bpk::unpack(const dByte *unpackFrom, dStr *unpackTo, size_t *sizeOut, size_t limit){
	ASRT_NOTNULL(unpackFrom); ASRT_NOTNULL(unpackTo); ASRT_NOTNULL(sizeOut);

	if(limit < sizeof(size_t))
		throw excep::underFlow(__FILE__, __LINE__);

	size_t numChars=0;
	memcpy(&numChars, unpackFrom, sizeof(size_t));

	(*sizeOut) = sizeof(size_t) + (numChars * sizeof(dNatChar));

	if(limit < (*sizeOut))
		throw excep::underFlow(__FILE__, __LINE__);

	if(numChars!=0)
		unpackTo->assign( reinterpret_cast<const dNatChar*>(unpackFrom+sizeof(size_t)), numChars);
	else
		unpackTo->clear();
}

void
bpk::pack(const dText *packMe, dByte **output, size_t *sizeOut){
	ASRT_NOTNULL(packMe); ASRT_NOTNULL(output);

	if(packMe->empty()){
		(*sizeOut) = sizeof(size_t);
	}else{
		(*sizeOut) = (packMe->length() + 1) * sizeof(dTextChar) + sizeof(size_t);
	}


	delete(*output);
	(*output) = new dByte[*sizeOut];
	*reinterpret_cast<size_t*>(*output) = packMe->length();

	if(!packMe->empty()){
		memcpy(
			(*output)+sizeof(size_t),
			packMe->data(),
			(*sizeOut) - sizeof(size_t)
		);
	}
}

void
bpk::unpack(const dByte *unpackFrom, dText *unpackTo, size_t *sizeOut, size_t limit){
	ASRT_NOTNULL(unpackFrom); ASRT_NOTNULL(unpackTo); ASRT_NOTNULL(sizeOut);

	if(limit < sizeof(size_t))
		throw excep::underFlow(__FILE__, __LINE__);

	size_t numChars=0;
	memcpy(&numChars, unpackFrom, sizeof(size_t));

	(*sizeOut) = sizeof(size_t) + (numChars * sizeof(dTextChar));

	if(limit < (*sizeOut))
		throw excep::underFlow(__FILE__, __LINE__);

	if(numChars!=0)
		unpackTo->assign( reinterpret_cast<const dTextChar*>(unpackFrom+sizeof(size_t)), numChars);
	else
		unpackTo->clear();
}

////////////////////////////////////////////////////////////
// Tests
#ifdef GTUT



#endif

