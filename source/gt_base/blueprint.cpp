//testing out eclipse file linking.

#include "blueprint.hpp"

using namespace gt;


////////////////////////////////////////////////////////////
cBlueprint::cBlueprint():
	mHash(0),
	mReplaces(uDoesntReplace),
	mFuncMake(NULL),
	mGetCom(NULL),
	mGecPlugTag(NULL),
	mGetName(NULL)
{}

cBlueprint::~cBlueprint(){
}

ptrFig
cBlueprint::make() const{
	ASRT_NOTNULL(mFuncMake);

	return mFuncMake();
}

dNameHash
cBlueprint::hash() const{
	return mHash;
}

const char* 
cBlueprint::name() const{
	return mGetName();
}

dNameHash
cBlueprint::replace() const{
	return mReplaces;
}

const cCommand*
cBlueprint::getCom(dNameHash pHash) const{
	ASRT_NOTNULL(mGetCom);
	return mGetCom(pHash);
}

const cPlugTag* 
cBlueprint::gecPlugTag(dNameHash pPT) const{
	ASRT_NOTNULL(mGecPlugTag);
	return mGecPlugTag(pPT);
}

void
cBlueprint::addToGivenContainers(dComContainer* pComContain, dPTagContainer* pPTagContain) const{
	ASRT_NOTNULL(mComContainRef);
	ASRT_NOTNULL(mPTagContainRef);

	pComContain->insert(mComContainRef->begin(), mComContainRef->end());
	pPTagContain->insert(mPTagContainRef->begin(), mPTagContainRef->end());
}

const cBlueprint*
cBlueprint::operator = (const cBlueprint* pCopy){
	PROFILE;

	ASRT_NOTNULL(pCopy);

	if(pCopy != this)
	{
		mHash = pCopy->mHash;
		mReplaces = pCopy->mReplaces;
		mGecPlugTag = pCopy->mGecPlugTag;

		//- Unsure if these need to be freed first.
		mFuncMake = pCopy->mFuncMake;
		mGetCom = pCopy->mGetCom;
		mComContainRef = pCopy->mComContainRef;
		mPTagContainRef = pCopy->mPTagContainRef;
	}
	return this;
}
