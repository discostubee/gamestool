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

#include "world.hpp"

using namespace gt;


////////////////////////////////////////////////////////////
cBlueprint::cBlueprint():
	mHash(0),
	mReplaces(uDoesntReplace),
	mExtends(uDoesntExtend),
	mFuncMake(NULL),
	mGetName(NULL),
	mGetCom(NULL),
	mGetPlugTag(NULL),
	mGetAllComs(NULL),
	mGetAllTags(NULL),
	mGetExtensions(NULL),
	mHasPlugTag(NULL)
{}

cBlueprint::~cBlueprint(){
}

ptrFig
cBlueprint::make(){
	ASRT_NOTNULL(mFuncMake);
	ptrFig rtn = mFuncMake();
	iniFig(rtn);
	return rtn;
}

dNameHash
cBlueprint::hash() const{
	return mHash;
}

const dPlaChar*
cBlueprint::name() const{
	return mGetName();
}

dNameHash
cBlueprint::replace() const{
	return mReplaces;
}

dNameHash
cBlueprint::extend() const{
	return mExtends;
}

const cCommand*
cBlueprint::getCom(dNameHash pHash) const{
	ASRT_NOTNULL(mGetCom);
	return mGetCom(pHash);
}

const cPlugTag* 
cBlueprint::getPlugTag(dNameHash pPT) const{
	ASRT_NOTNULL(mGetPlugTag);
	return mGetPlugTag(pPT);
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

dExtensions
cBlueprint::getExtensions() const{
	ASRT_NOTNULL(mGetExtensions);
	return mGetExtensions();
}

bool
cBlueprint::hasPlugTag(cPlugTag::dUID pPT) const{
	ASRT_NOTNULL(mHasPlugTag);
	return mHasPlugTag(pPT);
}

const cBlueprint*
cBlueprint::operator = (const cBlueprint* pCopy){
	PROFILE;

	ASRT_NOTNULL(pCopy);

	if(pCopy != this)
	{
		mHash = pCopy->mHash;
		mReplaces = pCopy->mReplaces;
		mExtends = pCopy->mExtends;
		mFuncMake = pCopy->mFuncMake;
		mGetName = pCopy->mGetName;
		mGetCom = pCopy->mGetCom;
		mGetPlugTag = pCopy->mGetPlugTag;
		mGetAllComs = pCopy->mGetAllComs;
		mGetAllTags = pCopy->mGetAllTags;
		mGetExtensions = pCopy->mGetExtensions;
		mHasPlugTag = pCopy->mHasPlugTag;
	}
	return this;
}

void
cBlueprint::setup(
	dNameHash pHash,
	dNameHash pReplaces,
	dNameHash pExtends,
	ptrFig (*pFuncMake)(),
	const dPlaChar* (*pGetName)(),
	const cCommand* (*pGetCom)(cCommand::dUID),
	const cPlugTag* (*pGetPlugTag)(cPlugTag::dUID),
	dListComs (*pGetAllComs)(),
	dListPTags (*pGetAllTags)(),
	dExtensions (*pGetExtensions)(),
	bool (*pHasPlugTag)(cPlugTag::dUID)
){
	mHash = pHash;
	mReplaces = pReplaces;
	mExtends = pExtends;
	mFuncMake = pFuncMake;
	mGetName = pGetName;
	mGetCom = pGetCom;
	mGetPlugTag = pGetPlugTag;
	mGetAllComs = pGetAllComs;
	mGetAllTags = pGetAllTags;
	mGetExtensions = pGetExtensions;
	mHasPlugTag = pHasPlugTag;

	DBUG_LO("blueprint '" << mGetName() << "' setup.");
}


void
cBlueprint::iniFig(ptrFig &iniMe){
	iniMe->ini(this, iniMe.getDir());
}
