/*
 * !\file
 * !\brief
 */

#ifndef BINPACKER_HPP
#define BINPACKER_HPP

#include "utils.hpp"

namespace bpk{

	//!\brief
	template<typename TYPE>
	void pack(const TYPE *packMe, dByte **output, size_t *sizeOut){
		//todo actual packing.
		ASRT_NOTNULL(output);
		ASRT_NOTNULL(packMe);
		ASRT_NOTNULL(sizeOut);

		delete [] *output;
		*output = new char[sizeof(TYPE)];	//- should be 'new ::dByte[]', but it doesn't work.
		memcpy(*output, packMe, sizeof(TYPE));
		*sizeOut = sizeof(TYPE);
	}

	template<typename TYPE>
	void unpack(const dByte *unpackFrom, TYPE *unpackTo, size_t *sizeOut, size_t limit){
		//todo actual unpacking.
		if(limit < sizeof(TYPE)){
			sizeOut = 0;
			unpackTo = NULL;
			return;
		}

		ASRT_NOTNULL(unpackFrom);
		ASRT_NOTNULL(unpackTo);
		ASRT_NOTNULL(sizeOut);

		memcpy(unpackTo, unpackFrom, sizeof(TYPE));
		*sizeOut = sizeof(TYPE);
	}


	void pack(const dStr *packMe, dByte **output, size_t *sizeOut);
	void unpack(const dByte *unpackFrom, dStr *unpackTo, size_t *sizeOut, size_t limit);
	void pack(const dText *packMe, dByte **output, size_t *sizeOut);
	void unpack(const dByte *unpackFrom, dText *unpackTo, size_t *sizeOut, size_t limit);
}

#endif
