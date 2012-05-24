
#include "binPacker.hpp"

template<typename CHAR_TYPE>
void packString(
	const typename std::basic_string< CHAR_TYPE, std::char_traits<CHAR_TYPE> > *packMe,
	dByte **output,
	size_t *sizeOut
){
	ASRT_NOTNULL(packMe); ASRT_NOTNULL(output);

	if(packMe->empty()){
		(*sizeOut) = sizeof(size_t);
	}else{
		(*sizeOut) = ((packMe->length() + 1) * sizeof(CHAR_TYPE)) + sizeof(size_t);
	}

	delete(*output);
	(*output) = new dByte[*sizeOut];
	*reinterpret_cast<size_t*>(*output) = packMe->length();

	if(!packMe->empty()){
		memcpy(
			(*output)+sizeof(size_t),
			packMe->data(),
			(*sizeOut) - sizeof(size_t) - sizeof(CHAR_TYPE)	//- std::string::data() doesn't use a null terminator
		);
	}
	memset(
		((*output)+sizeof(size_t)) + (packMe->length() * sizeof(CHAR_TYPE)),
		0,
		sizeof(CHAR_TYPE)
	);
}

template<typename CHAR_TYPE>
void unpackString(
	const dByte *unpackFrom,
	typename std::basic_string< CHAR_TYPE, std::char_traits<CHAR_TYPE> > *unpackTo,
	size_t *sizeOut,
	size_t limit
){
	ASRT_NOTNULL(unpackFrom); ASRT_NOTNULL(unpackTo); ASRT_NOTNULL(sizeOut);

	if(limit < sizeof(size_t))
		throw excep::underFlow(__FILE__, __LINE__);

	size_t numChars=0;
	memcpy(&numChars, unpackFrom, sizeof(size_t));

	if(numChars!=0)
		++numChars;	//- So we get the null terminator.

	(*sizeOut) = sizeof(size_t) + (numChars * sizeof(CHAR_TYPE));

	if(limit < (*sizeOut))
		throw excep::underFlow(__FILE__, __LINE__);

	if(numChars!=0){
		unpackTo->assign( reinterpret_cast<const CHAR_TYPE*>(unpackFrom+sizeof(size_t)), (numChars - 1));	//- Don't assign the null.
	}else{
		unpackTo->clear();
	}
}

void
bpk::pack(const dStr *packMe, dByte **output, size_t *sizeOut){
	packString(packMe, output, sizeOut);
}

void
bpk::unpack(const dByte *unpackFrom, dStr *unpackTo, size_t *sizeOut, size_t limit){
	unpackString(unpackFrom, unpackTo, sizeOut, limit);
}

void
bpk::pack(const dText *packMe, dByte **output, size_t *sizeOut){
	packString(packMe, output, sizeOut);
}

void
bpk::unpack(const dByte *unpackFrom, dText *unpackTo, size_t *sizeOut, size_t limit){
	unpackString(unpackFrom, unpackTo, sizeOut, limit);
}

////////////////////////////////////////////////////////////
// Tests
#ifdef GTUT



#endif

