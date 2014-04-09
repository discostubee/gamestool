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

template<typename CONT_T, typename CHAR_T>
void strPack(const CONT_T *packMe, dByte output[], size_t *sizeOut, size_t outLimit){
	ASRT_NOTNULL(packMe); ASRT_NOTNULL(sizeOut);

	(*sizeOut) = sizeof(size_t);

	if(!packMe->empty())
		(*sizeOut) += (packMe->length() * sizeof(CHAR_T));

	if(outLimit < *sizeOut)
		throw excep::underFlow(__FILE__, __LINE__);

	*reinterpret_cast<size_t*>(output) = packMe->length();
	if(packMe->length() == 0)
		return;

	memcpy(&output[sizeof(size_t)], packMe->data(), packMe->length() * sizeof(CHAR_T));
}

template<typename CONT_T, typename CHAR_T>
void strPack(const CONT_T *packMe, dByte **output, size_t *sizeOut){
	ASRT_NOTNULL(packMe); ASRT_NOTNULL(sizeOut);

	(*sizeOut) = sizeof(size_t);

	if(!packMe->empty())
		(*sizeOut) += (packMe->length() * sizeof(CHAR_T));

	delete [] (*output);
	(*output) = new dByte[*sizeOut];

	*reinterpret_cast<size_t*>(*output) = packMe->length();
	if(packMe->length() == 0)
		return;

	memcpy(&(*output)[sizeof(size_t)], packMe->data(), packMe->length() * sizeof(CHAR_T));
}

//!\brief
template<typename CONT_T, typename CHAR_T>
void strUnpack(const dByte *unpackFrom, CONT_T *unpackTo, size_t *sizeOut, size_t limit){
	ASRT_NOTNULL(unpackFrom); ASRT_NOTNULL(unpackTo); ASRT_NOTNULL(sizeOut);

	*sizeOut = sizeof(size_t);

	if(limit < *sizeOut)
		throw excep::underFlow(__FILE__, __LINE__);

	size_t numChars=0;
	memcpy(&numChars, unpackFrom, sizeof(size_t));

	if(numChars == 0)
		return;

	*sizeOut += sizeof(CHAR_T) * numChars;

	if(limit < *sizeOut)
		throw excep::underFlow(__FILE__, __LINE__);

	CHAR_T const *tmp = &unpackFrom[sizeof(size_t)];

	unpackTo->reserve(numChars);
	for(size_t i=0; i < numChars; ++i)
		unpackTo->push_back(tmp[i]);
}



void
bpk::pack(const dNatStr *packMe, dByte **output, size_t *sizeOut){
	strPack<dNatStr_def, dNatChar>(&packMe->t, output, sizeOut);
}

void
bpk::pack(const dNatStr *packMe, dByte output[], size_t *sizeOut, size_t outLimit){
	strPack<dNatStr_def, dNatChar>(&packMe->t, output, sizeOut, outLimit);
}

void
bpk::unpack(dNatStr *unpackTo, const dByte *unpackFrom, size_t *sizeOut, size_t limit){
	strUnpack<dNatStr_def, dNatChar>(unpackFrom, &unpackTo->t, sizeOut, limit);
}



void
bpk::pack(const dStr *packMe, dByte **output, size_t *sizeOut){
	ASRT_NOTNULL(packMe); ASRT_NOTNULL(output);

	dText tmp = toText(packMe->c_str());
	pack(&tmp, output, sizeOut);
}

void
bpk::pack(const dStr *packMe, dByte output[], size_t *sizeOut, size_t outLimit){
	ASRT_NOTNULL(packMe); ASRT_NOTNULL(sizeOut);

	dText tmp = toText(packMe->c_str());
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
bpk::pack(const dText *packMe, dByte **output, size_t *sizeOut){
	//- For now we are duplicating how the native string packer works, until more is understood about the best way to serialize UTF8
	strPack<dText_def, dTextChar>(&packMe->t, output, sizeOut);
}

void
bpk::pack(const dText *packMe, dByte output[], size_t *sizeOut, size_t outLimit){
	strPack<dText_def, dTextChar>(&packMe->t, output, sizeOut, outLimit);
}

void
bpk::unpack(dText *unpackTo, const dByte *unpackFrom, size_t *sizeOut, size_t limit){
	strUnpack<dText_def, dTextChar>(unpackFrom, &unpackTo->t, sizeOut, limit);
}

////////////////////////////////////////////////////////////
// Tests
#ifdef GTUT

GTUT_START(test_packer, intPacking){
	int meaning = 42, reply = 0;
	dByte *buff = NULL;
	size_t sizeBuff = 0, sizeUnpack = 0;

	bpk::pack(&meaning, &buff, &sizeBuff);
	bpk::unpack(&reply, buff, &sizeUnpack, sizeBuff);
	GTUT_ASRT(meaning == reply, "failed to pack/unpack an int");

	delete [] buff;
}GTUT_END;

GTUT_START(test_packer, nativeStringPackUnpack){
	dNatStr packMe, unpackMe;
	dByte *buff = NULL;
	size_t sizeBuff = 0, unpackSize = 0;

	packMe = "test me";
	bpk::pack(&packMe, &buff, &sizeBuff);
	bpk::unpack(&unpackMe, buff, &unpackSize, sizeBuff);
	GTUT_ASRT(packMe.t.compare(unpackMe.t)==0, "failed packing or unpacking");
	delete [] buff;
}GTUT_END;

GTUT_START(test_packer, platformStringPackUnpack){
	dStr packMe = "てすと";
	dStr unpackMe;
	dByte *buff = NULL;
	size_t sizeBuff = 0, unpackSize = 0;

	bpk::pack(&packMe, &buff, &sizeBuff);
	bpk::unpack(&unpackMe, buff, &unpackSize, sizeBuff);
	GTUT_ASRT(packMe.compare(unpackMe)==0, "failed packing or unpacking");
	delete [] buff;
}GTUT_END;

GTUT_START(test_packer, textStringPackUnpack){
	dText packMe, unpackMe;
	dByte *buff = NULL;
	size_t sizeBuff = 0, unpackSize = 0;

	packMe = "てすと";
	bpk::pack(&packMe, &buff, &sizeBuff);
	bpk::unpack(&unpackMe, buff, &unpackSize, sizeBuff);
	GTUT_ASRT(packMe.t.compare(unpackMe.t)==0, "failed packing or unpacking");
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

