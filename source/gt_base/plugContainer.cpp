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

#include "plugContainerOps.hpp"

////////////////////////////////////////////////////////////
using namespace gt;
cBase_plugContainer::cBase_plugContainer(){
}

cBase_plugContainer::~cBase_plugContainer(){
}


////////////////////////////////////////////////////////////
// Tests
#ifdef GTUT

GTUT_START(testPlugLinier, assignAppend){
	tPlugLinearContainer<int, std::vector> testArray;
	tPlugLinearContainer<int, std::list> testList;
	std::vector<int> a;
	std::list<int> b;

	a.push_back(1);
	a.push_back(2);
	a.push_back(3);

	b.push_back(4);
	b.push_back(5);
	b.push_back(6);

	testArray = a;
	testList = b;
	cBase_plug &refPlug = testList;
	testArray += refPlug;

	int num=1;
	for(tPlugLinearContainer<int, std::vector>::dItr itr = testArray.getItr(); itr.stillGood(); ++itr){
		GTUT_ASRT(itr.get().get() == num, "Not the right number");
		++num;
	}

	testArray = a;
	refPlug = testArray;
	testList = refPlug;
	testList += b;

	num=1;
	for(tPlugLinearContainer<int, std::list>::dItr itr = testList.getItr(); itr.stillGood(); ++itr){
		GTUT_ASRT(itr.get().get() == num, "Not the right number");
		++num;
	}

}GTUT_END;

GTUT_START(testPlugLinier, saveLoad){
	typedef tPlugLinearContainer<int, std::vector> dTestVec;
	typedef tPlugLinearContainer<int, std::list> dTestList;
	dTestVec vec;
	dTestList list;
	tPlug<int> A(1), B(2), C(3);
	cByteBuffer saved;
	dReloadMap dontcare;

	{
		vec += A;
		vec += B;
		vec += C;
		vec.save(&saved);
		vec.clear();
		GTUT_ASRT(vec.getCount() == 0, "didn't clear");
		vec.loadEat(&saved, &dontcare);
		dTestVec::dItr itr=vec.getItr();
		GTUT_ASRT(itr.stillGood(), "iterator bad");
		GTUT_ASRT(*itr == A, "A isn't right");
		++itr;
		GTUT_ASRT(itr.stillGood(), "iterator bad");
		GTUT_ASRT(*itr == B, "B isn't right");
		++itr;
		GTUT_ASRT(itr.stillGood(), "iterator bad");
		GTUT_ASRT(*itr == C, "C isn't right");
	}

	{
		list += A;
		list += B;
		list += C;
		list.save(&saved);
		list.clear();
		GTUT_ASRT(list.getCount() == 0, "didn't clear");
		list.loadEat(&saved, &dontcare);
		dTestList::dItr itr=list.getItr();
		GTUT_ASRT(itr.stillGood(), "iterator bad");
		GTUT_ASRT(*itr == A, "A isn't right");
		++itr;
		GTUT_ASRT(itr.stillGood(), "iterator bad");
		GTUT_ASRT(*itr == B, "B isn't right");
		++itr;
		GTUT_ASRT(itr.stillGood(), "iterator bad");
		GTUT_ASRT(*itr == C, "C isn't right");
	}

}GTUT_END;

GTUT_START(testPlugLinier, single2list){
	tPlug<int> A(0), magic(3), prime(7);
	tPlugLinearContainer<int, std::list> contain;

	contain += A;
	contain += magic;
	contain += prime;
	tPlugLinearContainer<int, std::list>::dItr itr = contain.getItr();
	GTUT_ASRT(itr.stillGood(), "List underflow");
	GTUT_ASRT(itr.get() == A, "Didn't get A");
	++itr;
	GTUT_ASRT(itr.stillGood(), "List underflow");
	GTUT_ASRT(itr.get() == magic, "Didn't get magic");
	++itr;
	GTUT_ASRT(itr.stillGood(), "List underflow");
	GTUT_ASRT(itr.get() == prime, "Didn't get prime");

}GTUT_END;



#endif
