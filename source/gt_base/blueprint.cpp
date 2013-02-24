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

#include "blueprint.hpp"

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
	mHasPlugTag(NULL)
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
		mGetPlugTag = pCopy->mGetPlugTag;
		mFuncMake = pCopy->mFuncMake;
		mGetCom = pCopy->mGetCom;
		mGetAllComs = pCopy->mGetAllComs;
		mGetAllTags = pCopy->mGetAllTags;
		mHasPlugTag = pCopy->mHasPlugTag;
	}
	return this;
}
