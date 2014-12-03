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
	mCom(aCom)
{
	DBUG_TRACK_START("lead");

#	ifdef GT_THREADS
		mCurrentSig = SL_NO_ENTRY;
#	endif
}

cLead::cLead(const cLead &otherLead):
	mCom(otherLead.mCom)
{
	DBUG_TRACK_START("lead");

#	ifdef GT_THREADS
		mCurrentSig = SL_NO_ENTRY;
#	endif

	cLead *other = const_cast<cLead*>(&otherLead);

	for(dDataMap::iterator itr = other->mTaggedData.begin(); itr != other->mTaggedData.end(); ++itr){
		mTaggedData[itr->first] = itr->second;
		mTaggedData[itr->first]->linkLead(this);
	}
}

cLead::~cLead(){
	try{
		for(scrItr = mTaggedData.begin(); scrItr != mTaggedData.end(); ++scrItr){
			try{
				scrItr->second->unlinkLead(this);
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

	scrItr = mTaggedData.find(aTag->mID);
	if(scrItr != mTaggedData.end()){
		scrItr->second->unlinkLead(this);
		scrItr->second = linkMe;
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

	scrItr = mTaggedData.find(pTag->mID);
	if(scrItr == mTaggedData.end())
		return false;

#	ifdef GT_THREADS
		ASRT_TRUE(mCurrentSig != SL_NO_ENTRY, "Not current signature");
		scrItr->second->readShadow(setMe, mCurrentSig);
#	else
		*setMe = *scrItr->second;
#	endif

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

	scrItr = mTaggedData.find(aTag->mID);
	if(scrItr == mTaggedData.end())
		return false;

#	ifdef GT_THREADS
		ASRT_TRUE(mCurrentSig != SL_NO_ENTRY, "Not current signature");
		scrItr->second->writeShadow(copyMe, mCurrentSig);
#	else
		*scrItr->second = *copyMe;
#	endif

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

	scrItr = mTaggedData.find(pTag->mID);
	if(scrItr == mTaggedData.end())
		return false;

#	ifdef GT_THREADS
		ASRT_TRUE(mCurrentSig != SL_NO_ENTRY, "Not current signature");
		scrItr->second->shadowAppends(addTo, mCurrentSig);
#	else
		*addTo += *scrItr->second;
#	endif

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

	scrItr = mTaggedData.find(pTag->mID);
	if(scrItr == mTaggedData.end())
		return false;

#	ifdef GT_THREADS
		ASRT_TRUE(mCurrentSig != SL_NO_ENTRY, "Not current signature");
		scrItr->second->appendShadow(addFrom, mCurrentSig);
#	else
		*scrItr->second += *addFrom;
#	endif

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

	scrItr = mTaggedData.find(aGetTag->mID);
	if(scrItr == mTaggedData.end()){
		return false;
	}

	if(aPutTag == NULL)
		aPutTag = aGetTag;

	passTo->linkPlug(scrItr->second, aPutTag);

	return true;
}

void
cLead::remPlug(const cPlugTag *pGetTag){
#	ifdef GT_THREADS
		dLock lock(mu);
#	endif

	scrItr = mTaggedData.find(pGetTag->mID);

	if(scrItr == mTaggedData.end())
		return;

	scrItr->second->unlinkLead(this);
}

void
cLead::unplug(cBase_plug* aPlug){
	PROFILE;

#	ifdef GT_THREADS
		dLock lock(mu);
#	endif

	//- Search for plug.
	scrItr = mTaggedData.begin();
	while(scrItr != mTaggedData.end()){
		if(scrItr->second == aPlug){
			dDataMap::iterator delMe = scrItr;
			++scrItr;
			mTaggedData.erase(delMe);
			if(scrItr == mTaggedData.end())
				break;
		}else{
			++scrItr;
		}
	}
}

bool
cLead::has(const cPlugTag *pTag){
	PROFILE;

#	ifdef GT_THREADS
		dLock lock(mu);
#	endif

	return mTaggedData.end() != mTaggedData.find(pTag->mID);
}



#ifdef GT_THREADS
	void
	cLead::start(dConSig pSig){
		ASRT_TRUE(mCurrentSig == SL_NO_ENTRY, "signature already set.");
		mCurrentSig = pSig;
	}

	void
	cLead::stop(){
		ASRT_TRUE(mCurrentSig != SL_NO_ENTRY, "Signature not set.");
		mCurrentSig = SL_NO_ENTRY;
	}
#endif


////////////////////////////////////////////////////////////
// Tests
#ifdef GTUT
using namespace gt;

#	ifdef GT_THREADS
		void gt::startLead(ptrLead lead, dConSig pSig){
			lead->start(pSig);
		}

		void gt::stopLead(ptrLead lead){
			lead->stop();
		}

		void gt::startLead(cLead &lead, dConSig sig){
			lead.start(sig);
		}

		void gt::stopLead(cLead &lead){
			lead.stop();
		}

#	else
		void gt::startLead(ptrLead lead, dConSig pSig){}
		void gt::stopLead(ptrLead lead){}
		void gt::startLead(cLead &lead, dConSig sig){}
		void gt::stopLead(cLead &lead){}
#	endif


GTUT_START(testLead, tagging){
	cContext fakeConx;
	cPlugTag tag("some tag");
	tActualCommand<cFigment> fakeCom(0, "don't care", 0, NULL);

	gWorld.get()->regContext(&fakeConx);	//- unreg-es on death.

	ptrLead tmpLead = gWorld.get()->makeLead(fakeCom.mID);

	tPlug<int> numA, numB;
	const int magic = 3;
	tmpLead->linkPlug(&numA, &tag);

	numA.get() = magic;

	{
		int testA=0;

		startLead(tmpLead, fakeConx.getSig());
			tmpLead->assignTo(&testA, &tag);
			GTUT_ASRT(testA == magic, "Lead didn't store A");

			tmpLead->copyPlug(&numB, &tag);
		stopLead(tmpLead);

		GTUT_ASRT(numB.get() == magic, "B didn't get A's number");
	}
}GTUT_END;


#ifdef GT_THREADS
GTUT_START(testLead, shadowUpdate){
}GTUT_END;
#endif

GTUT_START(testLead, appending){
	cContext fakeConx;
	cPlugTag tag("some tag");
	tActualCommand<cFigment> fakeCom(0, "don't care", 0, NULL);
	ptrLead testMe = gWorld.get()->makeLead(fakeCom.mID);
	tPlug<int> a(6);
	int b=3;

	gWorld.get()->regContext(&fakeConx);	//- unreg-es on death.

	startLead(testMe, fakeConx.getSig());
		testMe->linkPlug(&a, &tag);
		GTUT_ASRT(testMe->appendFrom(b, &tag), "append failed");
	stopLead(testMe);
	PLUG_REFRESH(a);
	GTUT_ASRT(a.get() == 9, "Didn't append");

	startLead(testMe, fakeConx.getSig());
		GTUT_ASRT(testMe->appendTo(&b, &tag), "append failed");
	stopLead(testMe);
	GTUT_ASRT(b==12, "Didn't append");

}GTUT_END;


#endif
