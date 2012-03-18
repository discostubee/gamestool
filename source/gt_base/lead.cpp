#include "lead.hpp"
#include "figment.hpp"

////////////////////////////////////////////////////////////
using namespace gt;

#ifdef GT_THREADS
	cUpdateLemming::cUpdateLemming(cBase_plug *callBack) : callMe(callBack)
	{}

	cUpdateLemming::~cUpdateLemming() {
		#ifdef GT_THREADS
			callMe->finishUpdate();
		#endif
	}
#endif

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

void
cBase_plug::linkLead(cLead *pLead){
	PROFILE;
	ASRT_NOTNULL(pLead);

	itrLead = mLeadsConnected.find(pLead);
	if(itrLead==mLeadsConnected.end()){
		mLeadsConnected[pLead] = 1;
	}else{
		++itrLead->second;
	}
}

void
cBase_plug::unlinkLead(cLead *pLead){
	PROFILE;
	ASRT_NOTNULL(pLead);

	itrLead = mLeadsConnected.find(pLead);
	if(itrLead != mLeadsConnected.end()){
		--itrLead->second;
		if(itrLead->second == 0){
			mLeadsConnected.erase(itrLead);
		}

	}else{
		WARN("lead isn't connected to this plug.");
	}
}



////////////////////////////////////////////////////////////
using namespace gt;


cLead::cLead(cCommand::dUID aCom, dConSig aConx):
	mCom(aCom), mConx(aConx)
{
	DBUG_TRACK_START("lead");
}

cLead::cLead(const cLead &otherLead):
	mCom(otherLead.mCom), mConx(otherLead.mConx)
{
	DBUG_TRACK_START("lead");

	cLead *other = const_cast<cLead*>(&otherLead);
	#ifdef GT_THREADS
			dLockLead lockMe(muLead);
			dLockLead lockOther(other->muLead);
	#endif

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
		#ifdef GT_THREADS
			dLockLead lock(muLead);
		#endif

		for(scrTDataItr = mTaggedData.begin(); scrTDataItr != mTaggedData.end(); ++scrTDataItr){
			scrTDataItr->second->unlinkLead(this);
		}

		for(scrPDataItr = mDataPile.begin(); scrPDataItr != mDataPile.end(); ++scrPDataItr){
			(*scrPDataItr)->unlinkLead(this);
		}

		DBUG_TRACK_END("lead");
	}catch(...){
		excep::base_error e("Unknown error when destroying a lead", __FILE__, __LINE__);
		WARN(e);
	}
}

cBase_plug*
cLead::getPlug(const cPlugTag* pTag){
	PROFILE;

	ASRT_NOTNULL(pTag);

	scrTDataItr = mTaggedData.find(pTag->mID);
	if(scrTDataItr == mTaggedData.end()){
		std::stringstream ss; ss << "plug " << pTag->mName;
		throw excep::notFound(ss.str().c_str(), __FILE__, __LINE__);
	}

	#ifdef GT_THREADS
		return scrTDataItr->second->getShadow(mConx, eSM_read);
	#else
		return scrTDataItr->second;
	#endif
}

void
cLead::addPlug(cBase_plug *addMe, const cPlugTag *aTag){
	PROFILE;

	ASRT_NOTNULL(addMe);	ASRT_NOTNULL(aTag);

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

void
cLead::setPlug(const cBase_plug *setMe, const cPlugTag *aTag, bool silentFail){
	PROFILE;

	ASRT_NOTNULL(setMe);	ASRT_NOTNULL(aTag);

	scrTDataItr = mTaggedData.find(aTag->mID);
	if(!silentFail && scrTDataItr == mTaggedData.end())
		throw excep::notFound("plug", __FILE__, __LINE__);

	#ifdef GT_THREADS
		*scrTDataItr->second->getShadow(mConx, eSM_write) = *setMe;
	#else
		*scrTDataItr->second = *setMe;
	#endif
}

void
cLead::addToPile(cBase_plug *addMe){
	addMe->linkLead(this);
	mDataPile.push_back(addMe);
}

void
cLead::unplug(cBase_plug* aPlug){
	#ifdef GT_THREADS
		dLockLead lock(muLead);
	#endif

	PROFILE;

	//- Search for plug.
	if(!mTaggedData.empty()){
		for(scrTDataItr = mTaggedData.begin(); scrTDataItr != mTaggedData.end(); ++scrTDataItr){
			if(scrTDataItr->second == aPlug){
				mTaggedData.erase(scrTDataItr);
			}
		}
	}

	if(!mDataPile.empty()){
		for(scrPDataItr = mDataPile.begin(); scrPDataItr != mDataPile.end(); ++scrPDataItr){
			if(*scrPDataItr == aPlug){
				mDataPile.erase(scrPDataItr);
			}
		}
	}
}



/*
void
cLead::add(cBase_plug* aPlug, const cPlugTag* pTag){
	add(aPlug, pTag->mID);
}

void
cLead::add(cBase_plug *aPlug, cPlugTag::dUID ID){
	PROFILE;

	scrTDataItr = mTaggedData.find(ID);
	if(scrTDataItr != mTaggedData.end()){
		scrTDataItr->second->unlinkLead(this);
		scrTDataItr->second = aPlug;
	}else{
		mTaggedData[ID] = aPlug;
	}
	aPlug->linkLead(this);
}

void
cLead::addToPile(cBase_plug* pData){
	PROFILE;

	mDataPile.push_back(pData);
	pData->linkLead(this);
}

cBase_plug*
cLead::getPlug(const cPlugTag* pTag){
	PROFILE;

	scrTDataItr = mTaggedData.find(pTag->mID);
	if(scrTDataItr == mTaggedData.end())
		throw excep::notFound(pTag->mName.c_str(), __FILE__, __LINE__);

	return scrTDataItr->second;
}

cLead::cPileItr
cLead::getPiledDItr(){
	return cPileItr(&mDataPile);
}


void
cLead::setPlug(cBase_plug *aPlug, const cPlugTag *aTag){
	*getPlug(aTag) = *aPlug;
}

void
cLead::setPlug(cBase_plug &aPlug, const cPlugTag *aTag){
	*getPlug(aTag) = aPlug;
}
*/





////////////////////////////////////////////////////////////
// Tests
#ifdef GTUT

using namespace gt;

GTUT_START(testLead, tagging){
	cContext fakeConx;
	cPlugTag tag("some tag");
	tActualCommand<cFigment> fakeCom(0, "don't care", 0, NULL);

	gWorld.get()->regContext(&fakeConx);	//- unreg-es on death.

	cLead lead(fakeCom.mID, fakeConx.getSig());

	tPlug<int> numA, numB;
	const int magic = 3;
	lead.addPlug(&numA, &tag);
	numA.mD = magic;
	{
		PLUGUP(numA);
	}

	numB = lead.getPlug(&tag);
	GTUT_ASRT(numB.mD == magic, "B didn't get A's number");

}GTUT_END;

GTUT_START(testLead, piling){
}GTUT_END;

GTUT_START(testLead, shadowUpdate){
	cContext conxA, conxB;
	tPlug<int> numA, numB;
	tActualCommand<cFigment> fakeCom(0, "don't care", 0, NULL);
	cPlugTag tag("some tag");
	cLead leadA(fakeCom.mID, conxA.getSig()), leadB(fakeCom.mID, conxB.getSig());
	const int magic = 3;
	const int magicSquare = magic*magic;

	GTUT_ASRT(conxA.getSig() != conxB.getSig(), "contexts have same signature");

	numA.mD = 0;

	leadA.addPlug(&numA, &tag);
	leadB.addPlug(&numA, &tag);
	numB.mD = magic;
	leadA.setPlug(&numB, &tag);
	{
		PLUGUP(numA);
		numA.mD *= numA.mD;
	}
	GTUT_ASRT(numA.mD == magicSquare, "A didn't get B's number");
	numB = leadB.getPlug(&tag);
	GTUT_ASRT(numB.mD == numA.mD, "something went wrong using multiple shadows");

}GTUT_END;

#endif
