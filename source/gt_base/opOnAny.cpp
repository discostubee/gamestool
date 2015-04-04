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


#include "opOnAny.hpp"



////////////////////////////////////////////////////////////
using namespace gt;

cAnyOp::cAnyOp(){
}

cAnyOp::~cAnyOp(){
	try{
		demerge();

	}catch(...){

	}
}

void
cAnyOp::addKat(iKat * pK){
	(void)getRef().mKats.insert(
		dMapKatTypes::value_type(pK->getType(), sKatOrig(pK))
	);
}

void
cAnyOp::setupAll(){
	static bool beenSetup=false;
	if(!beenSetup){
		for(dMapKatTypes::iterator itrKat = getRef().mKats.begin(); itrKat != getRef().mKats.end(); ++itrKat)
			itrKat->second.mKat->setup(this);

		beenSetup=true;
	}
}

void
cAnyOp::merge(cAnyOp * pOther){
	DBUG_VERBOSE_LO("Merging ops " << std::hex << &mKats << " to " << std::hex << &pOther->mKats);
	merge(&mKats, &pOther->mKats, this);
	merge(&pOther->mKats, &mKats, pOther);
	mLinkedAnyOps.push_back(pOther);
	pOther->mLinkedAnyOps.push_back(this);
}

void
cAnyOp::merge(dMapKatTypes *myKats, dMapKatTypes *yourKats, cAnyOp *me){
	dMapKatTypes::iterator itrK;
	dMapKatTypes::iterator found;

	for(itrK = myKats->begin(); itrK != myKats->end(); ++itrK){
		found = yourKats->find(itrK->first);
		if(found == yourKats->end()){
			found = yourKats->insert(
				yourKats->end(),
				dMapKatTypes::value_type(
					itrK->first,
					sKatOrig(
						itrK->second.mKat,
						me
					)
				)
			);
		}

		found->second.mKat->link(me, itrK->second.mKat);
	}
}

void
cAnyOp::demerge(){
	DBUG_VERBOSE_LO("Demerging all...");
	std::list<cAnyOp*> removeThese = mLinkedAnyOps;
	for(
		std::list<cAnyOp*>::iterator itrLinks = removeThese.begin();
		itrLinks != removeThese.end();
		++itrLinks
	){
		demerge(*itrLinks);
	}
}

void
cAnyOp::demerge(cAnyOp * pOther){
	ASRT_NOTNULL(pOther);

	cAnyOp * tmpAnyOp;
	cAnyOp * tmpMe;
	enum{ eStarted, eDidThisHeap, eDidOtherHeap } currentHeap = eStarted;

	do{
		switch(currentHeap){
			case eStarted:
				currentHeap = eDidThisHeap;
				tmpMe = this;
				break;
			case eDidThisHeap:
				currentHeap = eDidOtherHeap;
				tmpMe = pOther;
				pOther = this;
				break;
			default:
				ASRT_TRUE(false, "Bad state");
				break;
		}
		DBUG_VERBOSE_LO("Demerging " << std::hex << pOther << " form " << std::hex << tmpMe);

		dMapKatTypes::iterator itrMyK = tmpMe->mKats.begin();
		while(itrMyK != tmpMe->mKats.end()){
			itrMyK->second.mKat->unlink(pOther);
			tmpAnyOp = itrMyK->second.mOrig;
			if(tmpAnyOp != NULL){
				dMapKatTypes::iterator itrEraseMe = itrMyK;
				++itrMyK;
				tmpMe->mKats.erase(itrEraseMe);
			}else{
				++itrMyK;
			}
		}

		for(
			std::list<cAnyOp*>::iterator itrLinks = tmpMe->mLinkedAnyOps.begin();
			itrLinks != tmpMe->mLinkedAnyOps.end();
			++itrLinks
		){
			if(*itrLinks == pOther){
				tmpMe->mLinkedAnyOps.erase(itrLinks);
				break;
			}
		}

	}while(currentHeap != eDidOtherHeap);
}

cAnyOp&
cAnyOp::getRef(){
	static cAnyOp xOps;
	return xOps;
}

//////////////////////////////////////////////////////////////
#ifdef GTUT
#include "figment.hpp"

using namespace gt;

GTUT_START(test_anyOp, basic){
	tPlug<dStr> A("test");
	tPlug<dText> B;
	B = A;
}GTUT_END;

GTUT_START(test_anyOp, dllAdded){
	cContext dontcare;

	dRefWorld w = gWorld.get();

	tPlug<ptrFig> mesh = w->makeFig(makeHash("polymesh"));	//- Make sure it's drafted.
	tPlug<dText> text;
	tPlug<dText> result;

	ptrLead makeMesh = w->makeLead("polymesh", "add to mesh");
	text.get().t = "1,1,1;2,2,2;3,3,3";
	makeMesh->linkPlug(&text, w->getPlugTag("polymesh", "vertexes"));
	mesh.get()->jack(makeMesh, &dontcare);

	ptrLead getMesh = w->makeLead("polymesh", "get mesh");
	getMesh->linkPlug(&result, w->getPlugTag("polymesh", "vertexes"));
	mesh.get()->jack(getMesh, &dontcare);
	DBUG_LO(result.get().t);

}GTUT_END;

#endif
