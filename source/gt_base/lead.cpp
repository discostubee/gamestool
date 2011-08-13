#include "lead.hpp"
#include "figment.hpp"

////////////////////////////////////////////////////////////
using namespace gt;

cLead::cLead(const cCommand* pCom):
	mCom( const_cast<cCommand*>(pCom) )
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
cLead::add(cBase_plug* pData, const cPlugTag* pTag){
	PROFILE;

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
cLead::addToPile(cBase_plug* pData){
	PROFILE;

	mDataPile.push_back(pData);
	pData->linkLead(this);
}

void
cLead::take(cBase_plug* pData, const cPlugTag* pTag){
	PROFILE;

	if(!mCom->usesTag(pTag))
		throw excep::base_error("This command does not use that tag", __FILE__, __LINE__);

	scrTDataItr = mTaggedData.find(pTag->mID);
	if(scrTDataItr != mTaggedData.end()){
		scrTCleanItr = mTaggedCleanup.find(scrTDataItr);
		if(scrTCleanItr != mTaggedCleanup.end()){
			delete scrTDataItr->second;
		}else{ // Something was tagged perviously, but it's not managed by the lead.
			scrTDataItr->second->unlinkLead(this);
			mTaggedCleanup.insert(scrTDataItr);
		}
		scrTDataItr->second = pData;
	}else{
		mTaggedData.insert(	dDataMap::value_type(pTag->mID, pData) );
	}


	//- Don't bother linking, because the plug is destroyed with the lead.
}

void
cLead::takeToPile(cBase_plug* pData){
	PROFILE;

	mDataPile.push_back(pData);

	//!\todo Ugly solution, need to fix.
	scrPDataItr = mDataPile.begin();
	for(unsigned int i=0; i < mDataPile.size(); ++i)
		++scrPDataItr;

	mPileCleanup.insert(scrPDataItr);

	//- Don't bother linking, because the plug is destroyed with the lead.
}

cBase_plug*
cLead::getD(const cPlugTag* pTag){
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

////////////////////////////////////////////////////////////
// Tests
#ifdef GTUT

GTUT_START(testLead, plugLinking){
}GTUT_END;


#endif
