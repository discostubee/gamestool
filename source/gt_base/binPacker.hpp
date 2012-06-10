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
		if(limit < sizeof(TYPE))
			throw excep::underFlow(__FILE__, __LINE__);

		ASRT_NOTNULL(unpackFrom);
		ASRT_NOTNULL(unpackTo);
		ASRT_NOTNULL(sizeOut);

		memcpy(unpackTo, unpackFrom, sizeof(TYPE));
		*sizeOut = sizeof(TYPE);
	}


	//- Native ASCII packing
	void pack(const dNatStr *packMe, dByte **output, size_t *sizeOut);
	void unpack(const dByte *unpackFrom, dNatStr *unpackTo, size_t *sizeOut, size_t limit);

	//- Platform strings are converted to UTF-8 for packing.
	void pack(const dStr *packMe, dByte **output, size_t *sizeOut);
	void unpack(const dByte *unpackFrom, dStr *unpackTo, size_t *sizeOut, size_t limit);

	//- UTF-8 packing
	void pack(const dText *packMe, dByte **output, size_t *sizeOut);
	void unpack(const dByte *unpackFrom, dText *unpackTo, size_t *sizeOut, size_t limit);
}

#endif
