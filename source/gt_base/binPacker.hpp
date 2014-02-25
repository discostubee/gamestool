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

#ifndef BINPACKER_HPP
#define BINPACKER_HPP

#include "utils.hpp"

namespace bpk{

	//!\brief	Similar to the generic pack, but allows you to work with static-size arrays.
	template<typename TYPE, size_t LEN>
	void pack_a(const TYPE (&packMe)[LEN], dByte **output, size_t *sizeOut){
		ASRT_NOTNULL(output);
		ASRT_NOTNULL(sizeOut);

		delete [] *output;
		*output = new dByte[sizeof(TYPE)*LEN];
		memcpy(*output, packMe, sizeof(TYPE)*LEN);
		*sizeOut = sizeof(TYPE)*LEN;
	}

	//!\brief	Similar to the generic pack, but allows you to work with static-size arrays.
	template<typename TYPE, size_t LEN>
	void pack_a(const TYPE (&packMe)[LEN], dByte output[], size_t *sizeOut, size_t outLimit){
		ASRT_NOTNULL(sizeOut);

		if(outLimit < sizeof(TYPE)*LEN)
			throw excep::underFlow(__FILE__, __LINE__);

		memcpy(output, packMe, sizeof(TYPE)*LEN);
		*sizeOut = sizeof(TYPE)*LEN;
	}

	//!\brief	Similar to the generic unpack, but allows you to work with static-size arrays.
	template<typename TYPE, size_t LEN>
	void unpack_a(TYPE (&unpackTo)[LEN], const dByte *unpackFrom, size_t *sizeOut, size_t limit){
		ASRT_NOTNULL(unpackFrom);
		ASRT_NOTNULL(sizeOut);

		if(limit < sizeof(TYPE)*LEN)
			throw excep::underFlow(__FILE__, __LINE__);

		memcpy(unpackTo, unpackFrom, sizeof(TYPE)*LEN);
		*sizeOut = sizeof(TYPE)*LEN;
	}

	//!\brief	Takes a pointer to given memory location, and packs it into another location that is dynamically allocated.
	//!			You must clean up the output.
	//!\param packMe	The memory location to pack.
	//!\param output	The memory location is modified to point to dynamically allocated memory, which contains the packed version of the input.
	//!\param sizeOut	The number of byes used to allocate the output.
	//!\todo	Currently just binary fills, it's not taking big/small endian into account yet.
	template<typename TYPE>
	void pack(const TYPE *packMe, dByte **output, size_t *sizeOut){
		ASRT_NOTNULL(output);
		ASRT_NOTNULL(packMe);
		ASRT_NOTNULL(sizeOut);

		delete [] *output;
		*output = new dByte[sizeof(TYPE)];
		memcpy(*output, packMe, sizeof(TYPE));
		*sizeOut = sizeof(TYPE);
	}

	//!\brief	Similar to the other generic pack, but allows you to work with a pre-allocated buffer.
	//!\note	Throws if it can't fit the output in the provided buffer space.
	//!\param packMe	The memory location to pack.
	//!\param output	Pre allocated buffer.
	//!\param sizeOut	The number of bytes taken by the packing.
	//!\param outLimit	The size of the output buffer.
	//!\todo	Currently just binary fills, it's not taking big/small endian into account yet.
	template<typename TYPE>
	void pack(const TYPE *packMe, dByte output[], size_t *sizeOut, const size_t outLimit){
		ASRT_NOTNULL(packMe);
		ASRT_NOTNULL(sizeOut);

		if(outLimit < sizeof(TYPE))
			throw excep::underFlow(__FILE__, __LINE__);

		memcpy(output, packMe, sizeof(TYPE));
		*sizeOut = sizeof(TYPE);
	}

	//!\brief	Converts from a stores, cross platform type into a system type.
	//!\todo	Currently just binary fills, it's not taking big/small endian into account yet.
	template<typename TYPE>
	void unpack(TYPE *unpackTo, const dByte *unpackFrom, size_t *sizeOut, size_t limit){
		ASRT_NOTNULL(unpackFrom);
		ASRT_NOTNULL(unpackTo);
		ASRT_NOTNULL(sizeOut);

		if(limit < sizeof(TYPE))
			throw excep::underFlow(__FILE__, __LINE__);

		memcpy(unpackTo, unpackFrom, sizeof(TYPE));
		*sizeOut = sizeof(TYPE);
	}

	//--- Specialties.

	//- Native ASCII packing
	void pack(const dNatStr *packMe, dByte **output, size_t *sizeOut);
	void pack(const dNatStr *packMe, dByte output[], size_t *sizeOut, size_t outLimit);
	void unpack(dNatStr *unpackTo, const dByte *unpackFrom, size_t *sizeOut, size_t limit);

	//- Platform strings are converted to UTF-8 for packing.
	void pack(const dStr *packMe, dByte **output, size_t *sizeOut);
	void pack(const dStr *packMe, dByte output[], size_t *sizeOut, size_t outLimit);
	void unpack(dStr *unpackTo, const dByte *unpackFrom, size_t *sizeOut, size_t limit);

	//- UTF-8 packing
	void pack(const dText *packMe, dByte **output, size_t *sizeOut);
	void pack(const dText *packMe, dByte output[], size_t *sizeOut, size_t outLimit);
	void unpack(dText *unpackTo, const dByte *unpackFrom, size_t *sizeOut, size_t limit);
}

#endif
