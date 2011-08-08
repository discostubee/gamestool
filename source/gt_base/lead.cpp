#include "lead.hpp"
#include "figment.hpp"

////////////////////////////////////////////////////////////
using namespace gt;

cLead::cLead(const cCommand* pCom, dConSig aConx):
	mCom( const_cast<cCommand*>(pCom) ),
	mConx(aConx)
{
	DBUG_TRACK_START(mCom->mName.c_str());
	DBUG_VERBOSE_LO("Lead " << mCom->mName << " made");
}

cLead::~cLead(){
	try{
		DBUG_TRACK_END(mCom->mName.c_str());
		DBUG_VERBOSE_LO("Lead destroyed " << reinterpret_cast<unsigned int>(this));
		clear();
	}catch(...){
		excep::base_error e("Unknown error when destroying a lead", __FILE__, __LINE__);
		WARN(e);
	}
}

void
cLead::add(cBase_plug* pData, const tPlugTag* pTag, dConSig aCon){
	PROFILE;

	if(aCon != mConx)
		return;

	scrTDataItr = mTaggedData.find(pTag->mID);
	if(scrTDataItr != mTaggedData.end()){
		scrTCleanItr = mTaggedCleanup.find(scrTDataItr);
		if(scrTCleanItr != mTaggedCleanup.end()){
			delete scrTDataItr->second;
			mTaggedCleanup.erase(scrTCleanItr);
		}
		scrTDataItr->second->unlinkLead(this);
		scrTDataItr->second = pData;
	}else{
		mTaggedData[pTag->mID] = pData;
	}
	pData->linkLead(this);
}

void
cLead::addToPile(cBase_plug* pData, dConSig aCon){
	PROFILE;

	if(aCon != mConx)
		return;

	mDataPile.push_back(pData);
	pData->linkLead(this);
}

cBase_plug*
cLead::getPlug(const tPlugTag* pTag, dConSig aCon){
	PROFILE;

	if(aCon != mConx)
		return NULL;

	scrTDataItr = mTaggedData.find(pTag->mID);
	if(scrTDataItr == mTaggedData.end())
		throw excep::notFound(pTag->mName.c_str(), __FILE__, __LINE__);

	return scrTDataItr->second;
}

cLead::cPileItr
cLead::getPiledDItr(dConSig aCon){
	if(aCon != mConx)
		return cPileItr(NULL);

	return cPileItr(&mDataPile);
}


void
cLead::setPlug(cBase_plug *aPlug, const tPlugTag *aTag, dConSig aCon){
	*getPlug(aTag, aCon) = aPlug;
}

void
cLead::clear(){
	PROFILE;

	//- Shouldn't have to set to null as map.clear should call destructor on a pointer.
	for(scrTCleanItr = mTaggedCleanup.begin(); scrTCleanItr != mTaggedCleanup.end(); ++scrTCleanItr){
		delete (*scrTCleanItr)->second;
		mTaggedData.erase(*scrTCleanItr);
	}

	for(scrPCleanItr = mPileCleanup.begin(); scrPCleanItr != mPileCleanup.end(); ++scrPCleanItr){
		delete *(*scrPCleanItr);
		mDataPile.erase(*scrPCleanItr);
	}

	//- Unlink remaining plugs.
	for(scrTDataItr = mTaggedData.begin(); scrTDataItr != mTaggedData.end(); ++scrTDataItr){
		scrTDataItr->second->unlinkLead(this);
	}

	for(scrPDataItr = mDataPile.begin(); scrPDataItr != mDataPile.end(); ++scrPDataItr){
		(*scrPDataItr)->unlinkLead(this);
	}

	mTaggedCleanup.clear();
	mDataPile.clear();
}

void
cLead::unplug(cBase_plug* pPlug){
	PROFILE;

	//- Search for plug.
	if(!mTaggedData.empty()){
		for(scrTDataItr = mTaggedData.begin(); scrTDataItr != mTaggedData.end(); ++scrTDataItr){
			if(scrTDataItr->second == pPlug){

				//- Check if we need to clean it up.
				scrTCleanItr = mTaggedCleanup.find(scrTDataItr);
				if(scrTCleanItr != mTaggedCleanup.end()){
					delete scrTDataItr->second;
					mTaggedCleanup.erase(scrTCleanItr); // This should not call the destructor on a pointer so we shouldn't have to set it to null.
				}

				//- This should not call the destructor on a pointer.
				mTaggedData.erase(scrTDataItr);
				return;
			}
		}
	}

	//- If it wasn't in the tagged data.
	if(!mDataPile.empty()){
		for(scrPDataItr = mDataPile.begin(); scrPDataItr != mDataPile.end(); ++scrPDataItr){
			if(*scrPDataItr == pPlug){

				//- Check if we need to clean it up.
				scrPCleanItr = mPileCleanup.find(scrPDataItr);
				if(scrPCleanItr != mPileCleanup.end()){
					delete *scrPDataItr;
					mPileCleanup.erase(scrPCleanItr);
				}

				mDataPile.erase(scrPDataItr);
				return;
			}
		}
	}

}

////////////////////////////////////////////////////////////
// Tests
#ifdef GTUT

GTUT_START(testLead, plugLinking){
}GTUT_END;


#endif
