//testing out eclipse file linking.

#include "blueprint.hpp"

using namespace gt;


////////////////////////////////////////////////////////////
cBlueprint::cBlueprint():
	mHash(0),
	mReplaces(uDoesntReplace),
	mFuncMake(NULL),
	mGetName(NULL),
	mGetCom(NULL),
	mGecPlugTag(NULL),
	mGetAllComs(NULL),
	mGetAllTags(NULL)
{}

cBlueprint::~cBlueprint(){
}

ptrFig
cBlueprint::make(){
	ASRT_NOTNULL(mFuncMake);
	ptrFig tmp = mFuncMake();
	tmp->mBlueprint = this;
	return tmp;
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
cBlueprint::getPlugTag(dNameHash pPT) const{
	ASRT_NOTNULL(mGecPlugTag);
	return mGecPlugTag(pPT);
}

dListComs
cBlueprint::getAllComs() const{
	ASRT_NOTNULL(mGetAllComs);
	return mGetAllComs();
}

dListPTags
cBlueprint::getAllTags() const{
	ASRT_NOTNULL(mGetAllTags);
	return mGetAllTags();
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
		mFuncMake = pCopy->mFuncMake;
		mGetCom = pCopy->mGetCom;
		mGetAllComs = pCopy->mGetAllComs;
		mGetAllTags = pCopy->mGetAllTags;
	}
	return this;
}
