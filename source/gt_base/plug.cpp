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

////////////////////////////////////////////////////////////
using namespace gt;

void
gt::voidCopiers::textToNStr(const dText *pFrom, void *pTo){
	*reinterpret_cast<dNatStr*>(pTo) = ::toNStr(*pFrom);
}

void
gt::voidCopiers::textToPStr(const dText *pFrom, void *pTo){
	*reinterpret_cast<dStr*>(pTo) = ::toPStr(*pFrom);
}

////////////////////////////////////////////////////////////
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
// Tests
#ifdef GTUT

GTUT_START(testPlug, copyMem){
	const long magic = 3;

	tPlug<long> A;
	cBase_plug *refA = &A;
	long numA;

	A.get() = magic;
	refA->copyInto(&numA);
	GTUT_ASRT(numA == magic, "A didn't copy into the target correctly.");

}GTUT_END;

GTUT_START(testPlug, copyPlugs){
	const short magic = 3;

	tPlug<short> A, B;

	B.get() = magic;
	A = B;
	GTUT_ASRT(A.get() == magic, "A didn't copy B.");
}GTUT_END;

GTUT_START(testPlug, saveLoad){
	tPlug<int> A, B;
	cByteBuffer buff;
	dReloadMap dontCare;

	A = 42;
	A.save(&buff);
	B.loadEat(&buff, &dontCare);
	GTUT_ASRT(B.get() == A.get(), "A didn't save, or B didn't load, correctly.");
}GTUT_END;




#endif
