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

#include "figment.hpp"

////////////////////////////////////////////////////////////
using namespace gt;

cPlugTag::cPlugTag(const dPlaChar* pPlugName):
	mName(pPlugName),
	mID(makeHash( toNStr(mName) ))
{}

cPlugTag::~cPlugTag()
{}

cPlugTag& cPlugTag::operator = (const cPlugTag&){ return *this; }

////////////////////////////////////////////////////////////
using namespace gt;

const cCommandInfo::dUID cCommandInfo::noID = static_cast<cCommand::dUID>(-1);

cCommandInfo::cCommandInfo(
	const dUID pID,
	const char* pName,
	const dNameHash pParentHash
):
	mID(pID), mName(pName), mParent(pParentHash)
{}

cCommandInfo::cCommandInfo(const cCommandInfo &copyMe):
	mID(copyMe.mID), mName(copyMe.mName), mParent(copyMe.mParent)
{}

cCommandInfo::~cCommandInfo(){}

////////////////////////////////////////////////////////////
using namespace gt;

const cCommandInfo * cCommandContain::DUMMY = new cCommandInfo(cCommandInfo::noID, "dummy", 0);

cCommandContain::cCommandContain():
	mCom(DUMMY)
{}

cCommandContain::cCommandContain(const cCommandContain &copyMe):
	mCom(copyMe.mCom)
{}

cCommandContain::cCommandContain(const cCommandInfo *copyMe):
	mCom(mCom)
{}

cCommandContain& cCommandContain::operator=(const cCommandContain &copyMe){
	if(this != &copyMe){
		mCom = copyMe.mCom;
	}

	return *this;
}

cCommandContain& cCommandContain::operator=(const cCommandInfo *copyMe){
	mCom = copyMe;

	return *this;
}

cCommandContain& cCommandContain::operator+=(const cCommandContain &copyMe){
	return operator=(copyMe);
}

cCommandContain& cCommandContain::operator+=(const cCommandInfo *copyMe){
	return operator=(copyMe);
}

////////////////////////////////////////////////////////////
using namespace gt;

cCommand::cCommand(
	const dUID pID,
	const char* pName,
	const dNameHash pParentHash
):
	cCommandInfo(pID, pName, pParentHash)
{
	DBUG_TRACK_START("command");
}

cCommand::~cCommand(){
	DBUG_TRACK_END("command");
}


////////////////////////////////////////////////////////////
// Tests
#ifdef GTUT

class cComTestFig: public cFigment{
public:
	static const cPlugTag* xPT_testTag;

	static const dPlaChar* identify(){ return "command test figment"; }

	cComTestFig(){}
	~cComTestFig(){}
};

const cPlugTag* cComTestFig::xPT_testTag = tOutline<cComTestFig>::makePlugTag("test tag");

GTUT_START(command, nothing){

}GTUT_END;

#endif
