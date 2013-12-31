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

#include "plugContainer.hpp"

////////////////////////////////////////////////////////////
using namespace gt;
cBasePlugContainer::cBasePlugContainer(){

}

cBasePlugContainer::~cBasePlugContainer(){
}

cBase_plug::dPlugType
cBasePlugContainer::getType() const{
	return cBase_plug::genPlugType<cBasePlugContainer>();
}

////////////////////////////////////////////////////////////
// Tests
#ifdef GTUT

GTUT_START(testPlugLinier, assignAppend){
	tPlugLinearContainer<int, std::vector> testArray;
	tPlugLinearContainer<int, std::list> testList;
	std::vector<int> a;
	std::list<int> b;
	cBase_plug &refArray = testArray;
	cBase_plug &refList = testList;

	a.push_back(1);
	a.push_back(2);
	a.push_back(3);

	b.push_back(4);
	b.push_back(5);
	b.push_back(6);

	testArray = a;
	refArray += refList;

	refList = refArray;
	testList += b;

	int num=1;
	for(tPlugLinearContainer<int, std::vector>::dItr itr = testArray.getItr(); itr.stillGood(); ++itr){
		GTUT_ASRT(itr.get().get() == num, "Not the right number");
		++num;
	}

	num=1;
	for(tPlugLinearContainer<int, std::list>::dItr itr = testList.getItr(); itr.stillGood(); ++itr){
		GTUT_ASRT(itr.get().get() == num, "Not the right number");
		++num;
	}

}GTUT_END;

GTUT_START(testPlugLinier, saveLoad){
	tPlugLinearContainer<int, std::vector> vec;
	tPlugLinearContainer<int, std::list> list;
	tPlug<int> A(1), B(2), C(3);
	cByteBuffer saved;
	dReloadMap dontcare;

	vec += A;
	vec += B;
	vec += C;
	vec.save(&saved);
	vec.clear();
	GTUT_ASRT(vec.getCount() == 0, "didn't clear");
	vec.loadEat(&saved, &dontcare);
	GTUT_ASRT(*vec.getPlug(0) == A, "A isn't right");
	GTUT_ASRT(*vec.getPlug(1) == B, "B isn't right");
	GTUT_ASRT(*vec.getPlug(2) == C, "C isn't right");

	list += A;
	list += B;
	list += C;
	list.save(&saved);
	list.clear();
	GTUT_ASRT(list.getCount() == 0, "didn't clear");
	list.loadEat(&saved, &dontcare);
	GTUT_ASRT(*list.getPlug(0) == A, "A isn't right");
	GTUT_ASRT(*list.getPlug(1) == B, "B isn't right");
	GTUT_ASRT(*list.getPlug(2) == C, "C isn't right");

}GTUT_END;

GTUT_START(testPlugLinier, toSinglePlug){
	tPlug<int> A(0), magic(3), prime(7);
	tPlugLinearContainer<int, std::vector> contain;

	contain += magic;
	contain += prime;
	A = contain;
	GTUT_ASRT(A == magic, "Container didn't assign the right plug.");
}GTUT_END;

#include "figment.hpp"
GTUT_START(testLead, appendingLead){
	cContext fakeConx;
	cPlugTag tag("some tag");
	tActualCommand<cFigment> fakeCom(0, "don't care", 0, NULL);
	ptrLead testMe = gWorld.get()->makeLead(fakeCom.mID);
	tPlugLinearContainer<int, std::vector> plug;

	testMe->linkPlug(&plug, &tag);

	std::vector<int> a;
	std::list<int> b;
	for(int i=1; i <= 10; ++i){
		if(i < 6)
			a.push_back(i);
		else
			b.push_back(i);
	}

	gWorld.get()->regContext(&fakeConx);	//- unreg-es on death.

	std::vector<int> c;
	startLead(testMe, fakeConx.getSig());
		GTUT_ASRT(testMe->appendFrom(a, &tag), "Can't append with 'a'");
		GTUT_ASRT(testMe->appendFrom(b, &tag), "Can't append with 'b'");
		GTUT_ASRT(testMe->appendTo(&c, &tag), "Can't append to 'c'");
	stopLead(testMe);

	for(int i=0; i < 10; ++i){
		GTUT_ASRT(c.at(i)==i+1, "Didn't get the right return.");
	}

}GTUT_END;



#endif
