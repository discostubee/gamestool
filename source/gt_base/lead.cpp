#include "lead.hpp"
#include "figment.hpp"

////////////////////////////////////////////////////////////
using namespace gt;

cUpdateLemming::cUpdateLemming(cBase_plug *callBack) : callMe(callBack)
{}

cUpdateLemming::~cUpdateLemming() {
	callMe->finishUpdate();
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
{}

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

void
cLead::getPlug(cBase_plug* aOutPlug, const cPlugTag* pTag){
	PROFILE;

	scrTDataItr = mTaggedData.find(pTag->mID);
	if(scrTDataItr != mTaggedData.end()){
		*aOutPlug = scrTDataItr->second->getShadow(mConx, eSM_read);
	}else{
		WARN("can't find plug");
	}
}

void
cLead::addPlug(cBase_plug *aPlug, const cPlugTag *aTag){
	PROFILE;

	scrTDataItr = mTaggedData.find(aTag->mID);
	if(scrTDataItr != mTaggedData.end()){
		scrTDataItr->second->unlinkLead(this);
		scrTDataItr->second = aPlug;
	}else{
		mTaggedData[aTag->mID] = aPlug;
	}
	aPlug->linkLead(this);
}

void
cLead::setPlug(cBase_plug *aPlug, const cPlugTag *aTag){
	PROFILE;

	scrTDataItr = mTaggedData.find(aTag->mID);
	if(scrTDataItr != mTaggedData.end()){
		*scrTDataItr->second->getShadow(mConx, eSM_write) = aPlug;
	}else{
		WARN("can't find plug");
	}
}

void
cLead::unplug(cBase_plug* pPlug){
	#ifdef GT_THREADS
		dLockLead lock(muLead);
	#endif

	PROFILE;

	//- Search for plug.
	if(!mTaggedData.empty()){
		for(scrTDataItr = mTaggedData.begin(); scrTDataItr != mTaggedData.end(); ++scrTDataItr){
			if(scrTDataItr->second == pPlug){
				mTaggedData.erase(scrTDataItr);
			}
		}
	}

	if(!mDataPile.empty()){
		for(scrPDataItr = mDataPile.begin(); scrPDataItr != mDataPile.end(); ++scrPDataItr){
			if(*scrPDataItr == pPlug){
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
}GTUT_END;

GTUT_START(testLead, piling){
}GTUT_END;



#endif
