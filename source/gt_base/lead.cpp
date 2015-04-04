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

#include "lead.hpp"
#include "figment.hpp"

using namespace gt;

////////////////////////////////////////////////////////////

cLead::cLead(cCommand::dUID aCom):
	mCom(aCom), mConx(NULL)
{
	DBUG_TRACK_START("lead");
}

cLead::cLead(const cLead &otherLead):
	mCom(otherLead.mCom), mConx(NULL)
{
	DBUG_TRACK_START("lead");

	cLead *other = const_cast<cLead*>(&otherLead);
	for(dDataMap::iterator itr = other->mTaggedData.begin(); itr != other->mTaggedData.end(); ++itr){
		mTaggedData[itr->first] = itr->second;
		mTaggedData[itr->first]->linkLead(this);
	}
}

cLead::~cLead(){
	try{
#	ifdef GT_THREADS
		dLock lock(mu);
#	endif
		for(dDataMap::iterator itr = mTaggedData.begin(); itr != mTaggedData.end(); ++itr){
			try{
				itr->second->unlinkLead(this);
			}catch(excep::base_error &e){
				WARN(e);
			}
		}

		DBUG_TRACK_END("lead");
	}catch(excep::base_error &e){
		WARN(e);
	}catch(...){
		WARN_S("Unknown exception when destroying a lead");
	}
}

void
cLead::linkPlug(cBase_plug *linkMe, const cPlugTag *aTag){
	PROFILE;

	ASRT_NOTNULL(linkMe);
	ASRT_NOTNULL(aTag);

#	ifdef GT_THREADS
		dLock lock(mu);
#	endif

	dDataMap::iterator data = mTaggedData.find(aTag->mID);
	if(data != mTaggedData.end()){
		data->second->unlinkLead(this);
		data->second = linkMe;
	}else{
		mTaggedData[aTag->mID] =linkMe;
	}
	linkMe->linkLead(this);
}

bool
cLead::copyPlug(cBase_plug *setMe, const cPlugTag* pTag){
	PROFILE;

	ASRT_NOTNULL(pTag);

#	ifdef GT_THREADS
		dLock lock(mu);
#	endif

	dDataMap::iterator data = mTaggedData.find(pTag->mID);
	if(data == mTaggedData.end())
		return false;

	*setMe = *data->second;
	return true;
}

bool
cLead::setPlug(const cBase_plug *copyMe, const cPlugTag *aTag){
	PROFILE;

	ASRT_NOTNULL(copyMe);
	ASRT_NOTNULL(aTag);

#	ifdef GT_THREADS
		dLock lock(mu);
#	endif

	dDataMap::iterator data = mTaggedData.find(aTag->mID);
	if(data == mTaggedData.end())
		return false;

	*data->second = *copyMe;
	return true;
}

bool
cLead::appendPlug(cBase_plug *addTo, const cPlugTag *pTag){
	PROFILE;

	ASRT_NOTNULL(addTo);
	ASRT_NOTNULL(pTag);

#	ifdef GT_THREADS
		dLock lock(mu);
#	endif

	dDataMap::iterator data = mTaggedData.find(pTag->mID);
	if(data == mTaggedData.end())
		return false;

	*addTo += *data->second;
	return true;
}

bool
cLead::plugAppends(cBase_plug *addFrom,  const cPlugTag *pTag){
	PROFILE;

	ASRT_NOTNULL(addFrom);
	ASRT_NOTNULL(pTag);

#	ifdef GT_THREADS
		dLock lock(mu);
#	endif

	dDataMap::iterator data = mTaggedData.find(pTag->mID);
	if(data == mTaggedData.end())
		return false;

	*data->second += *addFrom;
	return true;
}

bool
cLead::passPlug(cLead *passTo, const cPlugTag *aGetTag, const cPlugTag *aPutTag){
	PROFILE;

	ASRT_NOTNULL(passTo);
	ASRT_NOTNULL(aGetTag);

#	ifdef GT_THREADS
		dLock lock(mu);
#	endif

	dDataMap::iterator data = mTaggedData.find(aGetTag->mID);
	if(data == mTaggedData.end()){
		return false;
	}

	if(aPutTag == NULL)
		aPutTag = aGetTag;

	passTo->linkPlug(data->second, aPutTag);

	return true;
}

void
cLead::remPlug(const cPlugTag *pGetTag){
#	ifdef GT_THREADS
		dLock lock(mu);
#	endif

	dDataMap::iterator data = mTaggedData.find(pGetTag->mID);
	if(data == mTaggedData.end())
		return;

	data->second->unlinkLead(this);
}

void
cLead::unplug(cBase_plug* aPlug){
	PROFILE;

#	ifdef GT_THREADS
		dLock lock(mu);
#	endif

	//- Search for plug.
	dDataMap::iterator itr = mTaggedData.begin();
	while(itr != mTaggedData.end()){
		if(itr->second == aPlug){
			dDataMap::iterator delMe = itr;
			++itr;
			mTaggedData.erase(delMe);
			if(itr == mTaggedData.end())
				break;
		}else{
			++itr;
		}
	}
}

bool
cLead::hasTag(const cPlugTag *pTag){
	PROFILE;

#	ifdef GT_THREADS
		dLock lock(mu);
#	endif

	return mTaggedData.end() != mTaggedData.find(pTag->mID);
}

void
cLead::startLead(cContext *pConx){
	ASRT_TRUE(mConx == NULL, "context already set.");
	mConx = pConx;
}


void
cLead::stopLead(){
	ASRT_TRUE(mConx != NULL, "context not set.");
	mConx = NULL;
}

//
cContext*
cLead::getCurrentContext(){
	ASRT_NOTNULL(mConx);
	return mConx;
}



////////////////////////////////////////////////////////////
// Tests
#ifdef GTUT
using namespace gt;


GTUT_START(testLead, tagging){
	cContext fakeConx;
	cPlugTag tag("some tag");
	tActualCommand<cFigment> fakeCom(0, "don't care", 0, NULL);

	gWorld.get()->regContext(&fakeConx);	//- unreg-es on death.

	ptrLead tmpLead = gWorld.get()->makeLead(fakeCom.mID);

	tPlug<int> numA, numB;
	tPlug<int> magic = 3;
	tmpLead->linkPlug(&numA, &tag);

	numA = magic;

	{
		tPlug<int> testA=0;

		tmpLead->copyPlug(&testA, &tag);
		GTUT_ASRT(testA == magic, "Lead didn't store A");
		tmpLead->copyPlug(&numB, &tag);
		GTUT_ASRT(numB == magic, "B didn't get A's number");
	}
}GTUT_END;

GTUT_START(testLead, appending){
	cContext fakeConx;
	cPlugTag tag("some tag");
	tActualCommand<cFigment> fakeCom(0, "don't care", 0, NULL);
	ptrLead testMe = gWorld.get()->makeLead(fakeCom.mID);
	tPlug<int> a(6);
	tPlug<int> b(3);

	gWorld.get()->regContext(&fakeConx);	//- unreg-es on death.

	testMe->linkPlug(&b, &tag);
	GTUT_ASRT(testMe->appendPlug(&a, &tag), "append failed");
	GTUT_ASRT(a.get() == 9, "Didn't append");

}GTUT_END;


#endif
