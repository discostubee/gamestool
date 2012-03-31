/*
 * !\file
 * !\brief
 */

#ifndef BINPACKER_HPP
#define BINPACKER_HPP

#include "utils.hpp"

namespace bpk{

	template<typename TYPE> void pack(const TYPE *packMe, void **output, size_t *sizeOut, size_t limit){
		//todo actual packing.
		if(limit < sizeof(TYPE)){
			sizeOut = 0;
			*output = NULL;
			return;
		}

		delete *output;
		*output = new dByte[sizeof(TYPE)];
		memcpy(*output, packMe, sizeof(TYPE));
		*sizeOut = sizeof(TYPE);
	}

	template<typename TYPE> void unpack(const void *unpackFrom, TYPE *unpackTo, size_t *sizeOut, size_t limit){
		//todo actual unpacking.
		if(limit < sizeof(TYPE)){
			sizeOut = 0;
			unpackTo = NULL;
			return;
		}

		memcpy(unpackFrom, unpackTo, sizeof(TYPE));
		*sizeOut = sizeof(TYPE);
	}

}

#endif
