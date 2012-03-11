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

#include "plug.hpp"
#include "figment.hpp"

using namespace gt;


cReload::cReload(){
}

cReload::cReload(ptrFig pFig, const dByte* copyMe, size_t buffSize): fig(pFig){
	if(copyMe != NULL && buffSize > 0)
		data.copy(copyMe, buffSize);
}

cReload::~cReload(){
}

////////////////////////////////////////////////////////////
cBase_plug::cBase_plug(PLUG_TYPE_ID pTI):
	mType(pTI)
{
}

cBase_plug::cBase_plug(const cBase_plug& pCopy):
	mType(pCopy.mType)
{
}

cBase_plug::~cBase_plug(){
}



////////////////////////////////////////////////////////////
// Tests
#ifdef GTUT

GTUT_START(testPlug, copy){
	tPlug<int> A;
	cBase_plug *B = new tPlug<int>();
	int C = 0;
	char D = 'a';
	
	A = 5;
	TRYME( *B = A );

	TRYME( B->copyInto(&C) );
	GTUT_ASRT(C == 5, "B didn't copy A");

	try{
		B->copyInto(&D);
	}catch(std::exception){
		GTUT_ASRT(true, "");
	}

	delete(B);
}GTUT_END;

GTUT_START(testPlug, saveLoad){
	tPlug<int> A, B;
	cByteBuffer buff;
	dReloadMap dontCare;

	A.mD = 42;
	A.save(&buff);
	B.loadEat(&buff, &dontCare);
	GTUT_ASRT(B.mD == A.mD, "A didn't save, or B didn't load, correctly.");
}GTUT_END;




#endif
