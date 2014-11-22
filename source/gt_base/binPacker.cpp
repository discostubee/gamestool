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

#include "binPacker.hpp"


const size_t bpk::MAX_STORESIZE = USHRT_MAX;


template<typename CONT_T, typename CHAR_T>
static void
strPack(const CONT_T *packMe, dByte output[], size_t *sizeOut, size_t outLimit){
	ASRT_NOTNULL(packMe); ASRT_NOTNULL(sizeOut);

	if(packMe->t.size() > bpk::MAX_STORESIZE)
		THROW_ERROR("String is too big.");

	if(packMe->t.size() == 0)
		(*sizeOut) = sizeof(bpk::dStoreSize);
	else
		(*sizeOut) = sizeof(bpk::dStoreSize) + packMe->t.size(); //- Empty stirng should append 0.

	if(outLimit < *sizeOut)
		throw excep::underFlow(__FILE__, __LINE__);

	if(packMe->t.size() == 0){
		memset(output, 0, sizeof(bpk::dStoreSize));
	}
	else{
		bpk::dStoreSize packLen = static_cast<bpk::dStoreSize>(packMe->t.size());
		memcpy(output, &packLen, sizeof(packLen));
		memcpy(&output[sizeof(packLen)], packMe->t.data(), packMe->t.size());
	}
}

template<typename CONT_T, typename CHAR_T>
static void
strPack(const CONT_T *packMe, dByte **output, size_t *sizeOut){
	ASRT_NOTNULL(packMe); ASRT_NOTNULL(sizeOut);

	if(packMe->t.size() > bpk::MAX_STORESIZE)
		THROW_ERROR("String is too big.");

	if(packMe->t.size() == 0)
		(*sizeOut) = sizeof(bpk::dStoreSize);
	else
		(*sizeOut) = sizeof(bpk::dStoreSize) + packMe->t.size(); //- Empty stirng should append 0.

	delete [] (*output);
	(*output) = new dByte[*sizeOut];

	if(packMe->t.size() == 0){
		memset((*output), 0, sizeof(bpk::dStoreSize));
	}
	else{
		bpk::dStoreSize packLen = static_cast<bpk::dStoreSize>(packMe->t.size());
		memcpy((*output), &packLen, sizeof(packLen));
		memcpy(&(*output)[sizeof(packLen)], packMe->t.data(), packMe->t.size());
	}
}

//!\brief
template<typename CONT_T, typename CHAR_T>
void
strUnpack(const dByte *unpackFrom, CONT_T *unpackTo, size_t *sizeOut, size_t limit){
	ASRT_NOTNULL(unpackFrom); ASRT_NOTNULL(unpackTo); ASRT_NOTNULL(sizeOut);

	bpk::dStoreSize numBytes=0;

	if(limit < sizeof(numBytes))
		throw excep::underFlow(__FILE__, __LINE__);

	memcpy(&numBytes, unpackFrom, sizeof(numBytes));

	if(numBytes == 0)
		return;

	if(limit < numBytes + sizeof(numBytes))
		throw excep::underFlow(__FILE__, __LINE__);

	(*sizeOut) = numBytes + sizeof(numBytes);
	CHAR_T const *tmp = static_cast<const CHAR_T*>(&unpackFrom[sizeof(numBytes)]);
	unpackTo->t.assign(tmp, numBytes / sizeof(CHAR_T));
}


void
bpk::pack(const dStr *packMe, dByte **output, size_t *sizeOut){
	ASRT_NOTNULL(packMe); ASRT_NOTNULL(output);

	dText tmp = toText(*packMe);
	pack(&tmp, output, sizeOut);
}

void
bpk::pack(const dStr *packMe, dByte output[], size_t *sizeOut, size_t outLimit){
	ASRT_NOTNULL(packMe); ASRT_NOTNULL(sizeOut);

	dText tmp = toText(*packMe);
	pack(&tmp, output, sizeOut, outLimit);
}

void
bpk::unpack(dStr *unpackTo, const dByte *unpackFrom, size_t *sizeOut, size_t limit){
	ASRT_NOTNULL(unpackFrom); ASRT_NOTNULL(unpackTo); ASRT_NOTNULL(sizeOut);

	dText tmp;
	unpack(&tmp, unpackFrom, sizeOut, limit);
	*unpackTo = toPStr(tmp);
}


void
bpk::pack(const dNatStr *packMe, dByte **output, size_t *sizeOut){
	strPack<dNatStr, dNatChar>(packMe, output, sizeOut);
}

void
bpk::pack(const dNatStr *packMe, dByte output[], size_t *sizeOut, size_t outLimit){
	strPack<dNatStr, dNatChar>(packMe, output, sizeOut, outLimit);
}

void
bpk::unpack(dNatStr *unpackTo, const dByte *unpackFrom, size_t *sizeOut, size_t limit){
	strUnpack<dNatStr, dNatChar>(unpackFrom, unpackTo, sizeOut, limit);
}


void
bpk::pack(const dText *packMe, dByte **output, size_t *sizeOut){
	//- For now we are duplicating how the native string packer works, until more is understood about the best way to serialize UTF8
	strPack<dText, dTextChar>(packMe, output, sizeOut);
}

void
bpk::pack(const dText *packMe, dByte output[], size_t *sizeOut, size_t outLimit){
	strPack<dText, dTextChar>(packMe, output, sizeOut, outLimit);
}

void
bpk::unpack(dText *unpackTo, const dByte *unpackFrom, size_t *sizeOut, size_t limit){
	strUnpack<dText, dTextChar>(unpackFrom, unpackTo, sizeOut, limit);
}

////////////////////////////////////////////////////////////
// Tests
#ifdef GTUT

GTUT_START(test_packer, intPacking){
	int meaning = 42, reply = 0;
	dByte *buff = NULL;
	size_t sizeBuff = 0, sizeUnpack = 0;
	try{
		bpk::pack(&meaning, &buff, &sizeBuff);
		bpk::unpack(&reply, buff, &sizeUnpack, sizeBuff);
		GTUT_ASRT(meaning == reply, "failed to pack/unpack an int");
	}catch(...){
		delete [] buff;
		throw;
	}

	delete [] buff;
}GTUT_END;

GTUT_START(test_packer, nativeStringPackUnpack){
	dNatStr packMe, unpackMe;
	dByte *buff = NULL;
	size_t sizeBuff = 0, unpackSize = 0;

	try{
		packMe = "test me";
		bpk::pack(&packMe, &buff, &sizeBuff);
		bpk::unpack(&unpackMe, buff, &unpackSize, sizeBuff);
		GTUT_ASRT(packMe.t.compare(unpackMe)==0, "failed packing or unpacking");
	}catch(...){
		delete [] buff;
		throw;
	}
	delete [] buff;
}GTUT_END;

GTUT_START(test_packer, platformStringPackUnpack){
	dStr packMe("test");
	dStr unpackMe;
	dByte *buff = NULL;
	size_t sizeBuff = 0, unpackSize = 0;

	try{
		bpk::pack(&packMe, &buff, &sizeBuff);
		bpk::unpack(&unpackMe, buff, &unpackSize, sizeBuff);
		GTUT_ASRT(packMe.compare(unpackMe)==0, "failed packing or unpacking");
	}catch(...){
		delete [] buff;
		throw;
	}
	delete [] buff;

}GTUT_END;

GTUT_START(test_packer, textStringPackUnpack){
	dText packMe, unpackMe;
	dByte *buff = NULL;
	size_t sizeBuff = 0, unpackSize = 0;

	packMe = "てすと";
	try{
		bpk::pack(&packMe, &buff, &sizeBuff);
		bpk::unpack(&unpackMe, buff, &unpackSize, sizeBuff);
		GTUT_ASRT(packMe.t.compare(unpackMe)==0, "failed packing or unpacking");
	}catch(...){
		delete [] buff;
		throw;
	}
	delete [] buff;
}GTUT_END;

GTUT_START(test_packer, arrayPack){
	const size_t NUM = 4;
	const size_t LIMIT = NUM * sizeof(size_t);
	size_t sizeOut = 0;
	size_t arrIn[NUM] = {0};
	size_t arrOut[NUM] = {0};
	dByte buff[LIMIT] = {0};

	for(size_t i=0; i < NUM; ++i)
		arrIn[i] = i;

	bpk::pack_a(arrIn, buff, &sizeOut, LIMIT);
	bpk::unpack_a(arrOut, buff, &sizeOut, LIMIT);
	for(size_t i=0; i < NUM; ++i){
		GTUT_ASRT(arrOut[i]==i, "didn't unpack right");
	}
}GTUT_END;

#endif

