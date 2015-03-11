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

//#include "figment.hpp"
//GTUT_START(testLead, appendingLead){
//	cContext fakeConx;
//	cPlugTag tag("some tag");
//	tActualCommand<cFigment> fakeCom(0, "don't care", 0, NULL);
//	ptrLead testMe = gWorld.get()->makeLead(fakeCom.mID);
//	tPlugLinearContainer<int, std::vector> plug;
//
//	testMe->linkPlug(&plug, &tag);
//
//	std::vector<int> a;
//	std::list<int> b;
//	for(int i=1; i <= 10; ++i){
//		if(i < 6)
//			a.push_back(i);
//		else
//			b.push_back(i);
//	}
//
//	gWorld.get()->regContext(&fakeConx);	//- unreg-es on death.
//
//	std::vector<int> c;
//	startLead(testMe, fakeConx.getSig());
//		GTUT_ASRT(testMe->appendFrom(a, &tag), "Can't append with 'a'");
//		GTUT_ASRT(testMe->appendFrom(b, &tag), "Can't append with 'b'");
//		GTUT_ASRT(testMe->assignTo(&c, &tag), "Can't append to 'c'");
//	stopLead(testMe);
//
//	for(int i=0; i < 10; ++i){
//		GTUT_ASRT(c.at(i)==i+1, "Didn't get the right return.");
//	}
//
//}GTUT_END;

//GTUT_START(testPlugLinier, dllAddedAnyOp){
//	std::cout << "hello A" << std::endl;
//
//	cContext dontcare;
//	dRefWorld w = gWorld.get();
//	tPlug<ptrFig> mesh = w->makeFig(makeHash("polymesh"));	//- Make sure it's drafted.
//	tPlug<dText> text;
//	tPlug<dText> result;
//
//	ptrLead makeMesh = w->makeLead("polymesh", "add to mesh");
//	text.get().t = "1,1,1;2,2,2;3,3,3";
//	makeMesh->linkPlug(&text, w->getPlugTag("polymesh", "vertexes"));
//	mesh.get()->jack(makeMesh, &dontcare);
//
//	ptrLead getMesh = w->makeLead("polymesh", "get mesh");
//	getMesh->linkPlug(&result, w->getPlugTag("polymesh", "vertexes"));
//	mesh.get()->jack(getMesh, &dontcare);
//	DBUG_LO(result.get().t);
//
//}GTUT_END;

#endif
