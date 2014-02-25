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
	mCleanup(NULL),
	mHash(0),
	mReplaces(uDoesntReplace),
	mExtends(uDoesntExtend),
	mMake(NULL),
	mUnmade(NULL),
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
	ASRT_NOTNULL(mMake);
	ptrFig rtn = mMake();
	iniFig(rtn);
	return rtn;
}

void
cBlueprint::unmade(){
	if(mUnmade == NULL)
		return;

	mUnmade();
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

void
cBlueprint::install(
	dNameHash pHash,
	dNameHash pReplaces,
	dNameHash pExtends,
	ptrFig (*pMake)(),
	void (*pUnmade)(),
	const dPlaChar* (*pGetName)(),
	const cCommand* (*pGetCom)(cCommand::dUID),
	const cPlugTag* (*pGetPlugTag)(cPlugTag::dUID),
	dListComs (*pGetAllComs)(),
	dListPTags (*pGetAllTags)(),
	dExtensions (*pGetExtensions)(),
	bool (*pHasPlugTag)(cPlugTag::dUID),
	void (*pCleanup)()
){
	mHash = pHash;
	mReplaces = pReplaces;
	mExtends = pExtends;
	mMake = pMake;
	mUnmade = pUnmade;
	mGetName = pGetName;
	mGetCom = pGetCom;
	mGetPlugTag = pGetPlugTag;
	mGetAllComs = pGetAllComs;
	mGetAllTags = pGetAllTags;
	mGetExtensions = pGetExtensions;
	mHasPlugTag = pHasPlugTag;
	mCleanup = pCleanup;

	DBUG_VERBOSE_LO("blueprint '" << mGetName() << "' installed.");
}


void
cBlueprint::iniFig(ptrFig &iniMe){
	iniMe->ini(this, iniMe.getDir());
}


////////////////////////////////////////////////////////////
