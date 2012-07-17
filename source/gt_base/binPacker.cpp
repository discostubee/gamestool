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

void
bpk::pack(const dNatStr *packMe, dByte **output, size_t *sizeOut){
	ASRT_NOTNULL(packMe); ASRT_NOTNULL(output);

	const dNatStr_def *ref = &packMe->t;

	if(ref->empty()){
		(*sizeOut) = sizeof(size_t);
	}else{
		(*sizeOut) = ((ref->length() + 1)) + sizeof(size_t);	//- Assume 8 bits per char
	}

	delete(*output);
	(*output) = new dByte[*sizeOut];
	*reinterpret_cast<size_t*>(*output) = ref->length();

	if(!ref->empty()){
		memcpy(
			(*output)+sizeof(size_t),
			ref->data(),
			(*sizeOut) - sizeof(size_t) - sizeof(dNatChar)	//- std::string::data() doesn't use a null terminator
		);
	}
	memset(
		((*output)+sizeof(size_t)) + ref->length(),	//- Assume 8 bits per char
		0,
		sizeof(dNatChar)
	);
}

void
bpk::unpack(const dByte *unpackFrom, dNatStr *unpackTo, size_t *sizeOut, size_t limit){
	ASRT_NOTNULL(unpackFrom); ASRT_NOTNULL(unpackTo); ASRT_NOTNULL(sizeOut);

	dNatStr_def *ref = &unpackTo->t;

	if(limit < sizeof(size_t))
		throw excep::underFlow(__FILE__, __LINE__);

	size_t numChars=0;
	memcpy(&numChars, unpackFrom, sizeof(size_t));

	if(numChars!=0)
		++numChars;	//- So we get the null terminator.

	(*sizeOut) = sizeof(size_t) + numChars;	//- Assume 8 bits per char

	if(limit < (*sizeOut))
		throw excep::underFlow(__FILE__, __LINE__);

	if(numChars!=0){
		ref->assign(
			reinterpret_cast<const dNatChar*>(
				unpackFrom+sizeof(size_t)
			), (numChars - 1)
		);	//- Don't assign the null.
	}else{
		ref->clear();
	}
}

void
bpk::pack(const dStr *packMe, dByte **output, size_t *sizeOut){
	ASRT_NOTNULL(packMe); ASRT_NOTNULL(output);

	dText tmp = toText(packMe->c_str());
	pack(&tmp, output, sizeOut);
}

void
bpk::unpack(const dByte *unpackFrom, dStr *unpackTo, size_t *sizeOut, size_t limit){
	ASRT_NOTNULL(unpackFrom); ASRT_NOTNULL(unpackTo); ASRT_NOTNULL(sizeOut);

	dText tmp;
	unpack(unpackFrom, &tmp, sizeOut, limit);
	*unpackTo = toPStr(tmp);
}

void
bpk::pack(const dText *packMe, dByte **output, size_t *sizeOut){
	ASRT_NOTNULL(packMe); ASRT_NOTNULL(output);

	//- For now we are duplicating how the native string packer works, until more is understood about the best way to serialize UTF8
	const dText_def *ref = &packMe->t;

	if(ref->empty()){
		(*sizeOut) = sizeof(size_t);
	}else{
		(*sizeOut) = ((ref->length() + 1)) + sizeof(size_t);
	}

	delete(*output);
	(*output) = new dByte[*sizeOut];
	*reinterpret_cast<size_t*>(*output) = ref->length();

	if(!ref->empty()){
		memcpy(
			(*output)+sizeof(size_t),
			ref->data(),
			(*sizeOut) - sizeof(size_t) - sizeof(dTextChar)
		);
	}
	memset(
		((*output)+sizeof(size_t)) + ref->length(),
		0,
		sizeof(dTextChar)
	);
}

void
bpk::unpack(const dByte *unpackFrom, dText *unpackTo, size_t *sizeOut, size_t limit){
	ASRT_NOTNULL(unpackFrom); ASRT_NOTNULL(unpackTo); ASRT_NOTNULL(sizeOut);

	dText_def *ref = &unpackTo->t;

	if(limit < sizeof(size_t))
		throw excep::underFlow(__FILE__, __LINE__);

	size_t numChars=0;
	memcpy(&numChars, unpackFrom, sizeof(size_t));

	if(numChars!=0)
		++numChars;	//- So we get the null terminator.

	(*sizeOut) = sizeof(size_t) + numChars;	//- Assume 8 bits per char

	if(limit < (*sizeOut))
		throw excep::underFlow(__FILE__, __LINE__);

	if(numChars!=0){
		ref->assign(
			reinterpret_cast<const dTextChar*>(
				unpackFrom+sizeof(size_t)
			), (numChars - 1)
		);	//- Don't assign the null.
	}else{
		ref->clear();
	}
}

////////////////////////////////////////////////////////////
// Tests
#ifdef GTUT

GTUT_START(test_string, intPacking){
	int meaning = 42, reply = 0;
	dByte *buff = NULL;
	size_t sizeBuff = 0, sizeUnpack = 0;

	bpk::pack(&meaning, &buff, &sizeBuff);
	bpk::unpack(buff, &reply, &sizeUnpack, sizeBuff);
	GTUT_ASRT(meaning == reply, "failed to pack/unpack an int");

	delete [] buff;
}GTUT_END;

GTUT_START(test_string, nativePackUnpack){
	dNatStr packMe, unpackMe;
	dByte *buff = NULL;
	size_t sizeBuff = 0, unpackSize = 0;

	packMe = "test me";
	bpk::pack(&packMe, &buff, &sizeBuff);
	bpk::unpack(buff, &unpackMe, &unpackSize, sizeBuff);
	GTUT_ASRT(packMe.t.compare(unpackMe.t)==0, "failed packing or unpacking");
	delete [] buff;
}GTUT_END;

GTUT_START(test_string, platformPackUnpack){
	dStr packMe = "てすと";
	dStr unpackMe;
	dByte *buff = NULL;
	size_t sizeBuff = 0, unpackSize = 0;

	bpk::pack(&packMe, &buff, &sizeBuff);
	bpk::unpack(buff, &unpackMe, &unpackSize, sizeBuff);
	GTUT_ASRT(packMe.compare(unpackMe)==0, "failed packing or unpacking");
	delete [] buff;
}GTUT_END;

GTUT_START(test_string, textPackUnpack){
	dText packMe, unpackMe;
	dByte *buff = NULL;
	size_t sizeBuff = 0, unpackSize = 0;

	packMe = "てすと";
	bpk::pack(&packMe, &buff, &sizeBuff);
	bpk::unpack(buff, &unpackMe, &unpackSize, sizeBuff);
	GTUT_ASRT(packMe.t.compare(unpackMe.t)==0, "failed packing or unpacking");
	delete [] buff;
}GTUT_END;

#endif

