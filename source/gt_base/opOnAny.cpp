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
			itrKat->second.mRefKat->setup(this);

		beenSetup=true;
	}
}

void
cAnyOp::merge(cAnyOp * pOther){
	DBUG_VERBOSE_LO("Merging ops " << std::hex << &mKats << " to " << std::hex << &pOther->mKats);
	merge(this, pOther);
	merge(pOther, this);
}

void
cAnyOp::merge(cAnyOp *me, cAnyOp *you){
	dMapKatTypes::iterator itrK;
	dMapKatTypes::iterator found;

	for(itrK = you->mKats.begin(); itrK != you->mKats.end(); ++itrK){
		found = me->mKats.find(itrK->first);
		if(found == me->mKats.end()){
			found = me->mKats.insert(
				me->mKats.end(),
				dMapKatTypes::value_type(
					itrK->first,
					sKatOrig(
						itrK->second.mRefKat,
						you
					)
				)
			);
		}

		found->second.mRefKat->link(me, itrK->second.mRefKat);
	}
	me->mLinkedAnyOps.push_back(you);
}

void
cAnyOp::demerge(){
	DBUG_VERBOSE_LO("Demerging all...");
	for(
		std::list<cAnyOp*>::iterator itrLinks = mLinkedAnyOps.begin();
		itrLinks != mLinkedAnyOps.end();
		++itrLinks
	){
		demerge(*itrLinks);
	}
}

void
cAnyOp::demerge(cAnyOp *pOther){
	demerge(this, pOther);
	demerge(pOther, this);
}

void
cAnyOp::demerge(cAnyOp *me, cAnyOp *you){
	ASRT_NOTNULL(me);
	ASRT_NOTNULL(you);

	DBUG_VERBOSE_LO("Demerging me(" << std::hex << me << ") from you(" << std::hex << you << ") :");
	for(
		std::list<cAnyOp*>::iterator itrLink = me->mLinkedAnyOps.begin();
		itrLink != me->mLinkedAnyOps.end();
		++itrLink
	){
		if(*itrLink == you){
			typename dMapKatTypes::iterator itrYourKats = you->mKats.begin();
			while(itrYourKats != you->mKats.end()){
				if(itrYourKats->second.mOrig == me){
					DBUG_VERBOSE_LO("   removing my op " << itrYourKats->second.mRefKat->getName());
					itrYourKats->second.mRefKat->unlink(you);
					typename dMapKatTypes::iterator delMe = itrYourKats;
					++itrYourKats;
					you->mKats.erase(delMe);

				}else{
					DBUG_VERBOSE_LO("   removing your op " << itrYourKats->second.mRefKat->getName());
					itrYourKats->second.mRefKat->unlink(me);
					++itrYourKats;
				}
			}
			you->mLinkedAnyOps.erase(itrLink);
			return;
		}
	}
	DBUG_LO("   Did't find you");
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
