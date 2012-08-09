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
#ifdef GT_THREADS
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

#else
	void gt::startLead(ptrLead lead, dConSig pSig){}
	void gt::stopLead(ptrLead lead){}
	void startLead(cLead &lead, dConSig sig){}
	void stopLead(cLead &lead){}
#endif

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

	for(dPiledData::iterator itr = other->mDataPile.begin(); itr != other->mDataPile.end(); ++itr){
		mDataPile.push_back(*itr);
		mDataPile.back()->linkLead(this);
	}
}

cLead::~cLead(){
	try{
		for(scrTDataItr = mTaggedData.begin(); scrTDataItr != mTaggedData.end(); ++scrTDataItr){
			try{
				scrTDataItr->second->unlinkLead(this);
			}catch(excep::base_error &e){
				WARN(e);
			}
		}

		for(scrPDataItr = mDataPile.begin(); scrPDataItr != mDataPile.end(); ++scrPDataItr){
			try{
				(*scrPDataItr)->unlinkLead(this);
			}catch(excep::base_error &e){
				WARN(e);
			}
		}

		DBUG_TRACK_END("lead");
	}catch(std::exception &e){
		WARN_S("Destroying lead " << e.what());
	}catch(...){
		excep::base_error e("Unknown exception when destroying a lead", __FILE__, __LINE__);
		WARN(e);
	}
}

void
cLead::addPlug(cBase_plug *addMe, const cPlugTag *aTag){
	PROFILE;

	ASRT_NOTNULL(addMe);
	ASRT_NOTNULL(aTag);

#	ifdef GT_THREADS
		dLock unplugLock(mu);
#	endif

	scrTDataItr = mTaggedData.find(aTag->mID);
	if(scrTDataItr != mTaggedData.end()){
		scrTDataItr->second->unlinkLead(this);
		scrTDataItr->second = addMe;
	}else{
		//mTaggedData.insert( std::pair<cPlugTag::dUID, cBase_plug *>(aTag->mID, aPlug) );
		mTaggedData[aTag->mID] =addMe;
	}
	addMe->linkLead(this);
}

bool
cLead::getPlug(cBase_plug *setMe, const cPlugTag* pTag){
	PROFILE;

	ASRT_NOTNULL(pTag);

#	ifdef GT_THREADS
		dLock unplugLock(mu);
#	endif

	scrTDataItr = mTaggedData.find(pTag->mID);
	if(scrTDataItr == mTaggedData.end())
		return false;

#	ifdef GT_THREADS
		ASRT_TRUE(mCurrentSig != SL_NO_ENTRY, "Not current signature");
		scrTDataItr->second->readShadow(setMe, mCurrentSig);
#	else
		*setMe = *scrTDataItr->second;
#	endif

	return true;
}

bool
cLead::setPlug(const cBase_plug *setMe, const cPlugTag *aTag){
	PROFILE;

	ASRT_NOTNULL(setMe);
	ASRT_NOTNULL(aTag);

#	ifdef GT_THREADS
		dLock unplugLock(mu);
#	endif

	scrTDataItr = mTaggedData.find(aTag->mID);
	if(scrTDataItr == mTaggedData.end())
		return false;

#	ifdef GT_THREADS
		ASRT_TRUE(mCurrentSig != SL_NO_ENTRY, "Not current signature");
		scrTDataItr->second->writeShadow(setMe, mCurrentSig);
#	else
		*scrTDataItr->second = *setMe;
#	endif

	return true;
}

bool
cLead::passPlug(cLead *passTo, const cPlugTag *aGetTag, const cPlugTag *aPutTag){
	PROFILE;

	ASRT_NOTNULL(passTo);
	ASRT_NOTNULL(aGetTag);

#	ifdef GT_THREADS
		dLock unplugLock(mu);
#	endif

	scrTDataItr = mTaggedData.find(aGetTag->mID);
	if(scrTDataItr == mTaggedData.end()){
		return false;
	}

	if(aPutTag == NULL)
		aPutTag = aGetTag;

	passTo->addPlug(scrTDataItr->second, aPutTag);

	return true;
}

void
cLead::remPlug(const cPlugTag *pGetTag){
#	ifdef GT_THREADS
		dLock unplugLock(mu);
#	endif

	scrTDataItr = mTaggedData.find(pGetTag->mID);

	if(scrTDataItr == mTaggedData.end())
		return;

	scrTDataItr->second->unlinkLead(this);
}

void
cLead::addToPile(cBase_plug *addMe){
#	ifdef GT_THREADS
		dLock unplugLock(mu);
#	endif

	addMe->linkLead(this);
	mDataPile.push_back(addMe);
}

void
cLead::passToPile(cLead *passTo, const cPlugTag *pGetTag){
#	ifdef GT_THREADS
		dLock unplugLock(mu);
#	endif

	scrTDataItr = mTaggedData.find(pGetTag->mID);

	if(scrTDataItr == mTaggedData.end())
		return;

	scrTDataItr->second->linkLead(passTo);
	passTo->mDataPile.push_back(scrTDataItr->second);
}

void
cLead::addPile(cLead *passTo){
#	ifdef GT_THREADS
		dLock unplugLock(mu);
#	endif

	for(scrPDataItr = passTo->mDataPile.begin(); scrPDataItr != passTo->mDataPile.end(); ++scrPDataItr){
		mDataPile.push_back(*scrPDataItr);
		(*scrPDataItr)->linkLead(this);
	}
}

void
cLead::clearPile(){
#	ifdef GT_THREADS
		dLock unplugLock(mu);
#	endif

	for(scrPDataItr = mDataPile.begin(); scrPDataItr != mDataPile.end(); ++scrPDataItr)
		(*scrPDataItr)->unlinkLead(this);

	mDataPile.clear();
}

void
cLead::unplug(cBase_plug* aPlug){
	PROFILE;

#	ifdef GT_THREADS
		dLock unplugLock(mu);
#	endif

	//- Search for plug.
	for(scrTDataItr = mTaggedData.begin(); scrTDataItr != mTaggedData.end(); ++scrTDataItr){
		if(scrTDataItr->second == aPlug){
			mTaggedData.erase(scrTDataItr);
		}
	}

	for(scrPDataItr = mDataPile.begin(); scrPDataItr != mDataPile.end(); ++scrPDataItr){
		if(*scrPDataItr == aPlug){
			mDataPile.erase(scrPDataItr);
		}
	}
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

GTUT_START(testLead, tagging){
	cContext fakeConx;
	cPlugTag tag("some tag");
	tActualCommand<cFigment> fakeCom(0, "don't care", 0, NULL);

	gWorld.get()->regContext(&fakeConx);	//- unreg-es on death.

	ptrLead tmpLead = gWorld.get()->makeLead(fakeCom.mID);

	tPlug<int> numA, numB;
	const int magic = 3;
	tmpLead->addPlug(&numA, &tag);

	numA.get() = magic;

	{
		int testA=0;
		tmpLead->getValue(&testA, &tag);
		GTUT_ASRT(testA == magic, "Lead didn't store A");

		startLead(tmpLead, fakeConx.getSig());
		tmpLead->getPlug(&numB, &tag);
		stopLead(tmpLead);

		GTUT_ASRT(numB.get() == magic, "B didn't get A's number");
	}
}GTUT_END;

GTUT_START(testLead, piling){
}GTUT_END;

GTUT_START(testLead, shadowUpdate){
	#ifdef GT_TREADS
		cContext conxA, conxB;
		tPlug<int> numA, numB;
		tActualCommand<cFigment> fakeCom(0, "don't care", 0, NULL);
		cPlugTag tag("some tag");
		cLead leadA(fakeCom.mID, conxA.getSig()), leadB(fakeCom.mID, conxB.getSig());
		const int magic = 3;
		const int magicSquare = magic*magic;

		GTUT_ASRT(conxA.getSig() != conxB.getSig(), "contexts have same signature");

		numA.get() = 0;

		startLead(leadA, conxA.getSig());
		leadA.addPlug(&numA, &tag);
		leadA.setPlug(&numB, &tag);
		stopLead(leadA);

		startLead(leadB, conxB.getSig());
		leadB.addPlug(&numA, &tag);
		stopLead(leadB);

		numB.get() = magic;
		numB.updateStart();
		numB.updateFinish();

		numA.get() *= numA.get();
		numA.updateStart();
		numA.updateFinish();

		GTUT_ASRT(numA.get() == magicSquare, "A didn't get B's number");
		numB = leadB.getPlug(&tag);
		GTUT_ASRT(numB.get() == numA.get(), "something went wrong using multiple shadows");
	#endif
}GTUT_END;

#endif
