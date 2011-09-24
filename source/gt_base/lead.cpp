#include "lead.hpp"
#include "figment.hpp"

////////////////////////////////////////////////////////////
using namespace gt;

cLead::cLead(cCommand::dUID aCom, dConSig aConx):
	mCom( aCom ),
	mConx(aConx)
{
	DBUG_TRACK_START("lead");
}

cLead::~cLead(){
	try{
		DBUG_TRACK_END("lead");
		clear();
	}catch(...){
		excep::base_error e("Unknown error when destroying a lead", __FILE__, __LINE__);
		WARN(e);
	}
}

void
cLead::add(cBase_plug* aPlug, const cPlugTag* pTag, dConSig aCon){
	add(aPlug, pTag->mID, aCon);
}

void
cLead::add(cBase_plug *aPlug, cPlugTag::dUID ID, dConSig aCon){
	PROFILE;

	if(aCon != mConx)
		return;

	scrTDataItr = mTaggedData.find(ID);
	if(scrTDataItr != mTaggedData.end()){
		scrTCleanItr = mTaggedCleanup.find(scrTDataItr);
		if(scrTCleanItr != mTaggedCleanup.end()){
			delete scrTDataItr->second;
			mTaggedCleanup.erase(scrTCleanItr);
		}
		scrTDataItr->second->unlinkLead(this);
		scrTDataItr->second = aPlug;
	}else{
		mTaggedData[ID] = aPlug;
	}
	aPlug->linkLead(this);
}

void
cLead::addToPile(cBase_plug* pData, dConSig aCon){
	PROFILE;

	if(aCon != mConx)
		throw excep::badContext(__FILE__, __LINE__);

	mDataPile.push_back(pData);
	pData->linkLead(this);
}

cBase_plug*
cLead::getPlug(const cPlugTag* pTag, dConSig aCon){
	PROFILE;

	if(aCon != mConx)
		throw excep::badContext(__FILE__, __LINE__);

	scrTDataItr = mTaggedData.find(pTag->mID);
	if(scrTDataItr == mTaggedData.end())
		throw excep::notFound(pTag->mName.c_str(), __FILE__, __LINE__);

	return scrTDataItr->second;
}

cLead::cPileItr
cLead::getPiledDItr(dConSig aCon){
	if(aCon != mConx)
		throw excep::badContext(__FILE__, __LINE__);

	return cPileItr(&mDataPile);
}


void
cLead::setPlug(cBase_plug *aPlug, const cPlugTag *aTag, dConSig aCon){
	*getPlug(aTag, aCon) = *aPlug;
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

using namespace gt;

GTUT_START(testLead, addByTag){
	cContext fake;
	cPlugTag tag("tag");
	cLead testMe(0, &fake);
	tPlug<int> number;
	tPlug<int> target;

	number = 42;

	testMe.add(&number, &tag, &fake);
	target = testMe.getPlug(&tag, &fake);
	GTUT_ASRT(target.mD==42, "Lead didn't give back the right data.");
}GTUT_END;

GTUT_START(testLead, pile){
}GTUT_END;

GTUT_START(testLead, setPlug){
	cContext fake;
	cPlugTag tag("tag");
	cLead testMe(0, &fake);
	tPlug<int> number;
	tPlug<int> target;

	number = 42;
	testMe.add(&target, &tag, &fake);
	testMe.setPlug(&number, &tag, &fake);
	GTUT_ASRT(target.mD==42, "Lead didn't give back the right data.");
}GTUT_END;

GTUT_START(testLead, stopBadContext){
}GTUT_END;

#endif
