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

#include "alias.hpp"


////////////////////////////////////////////////////////////
using namespace gt;

const cPlugTag *cAlias::xPT_fig = tOutline<cAlias>::makePlugTag("figment");
const cPlugTag *cAlias::xPT_alias = tOutline<cAlias>::makePlugTag("alias name");

const cCommand::dUID cAlias::xBindFig = tOutline<cAlias>::makeCommand(
	"bind fig", &cAlias::patBindFig,
	cAlias::xPT_fig,
	NULL
);

const cCommand::dUID cAlias::xSetAlias = tOutline<cAlias>::makeCommand(
	"set alias", &cAlias::patSetAlias,
	cAlias::xPT_alias,
	NULL
);

cAlias::cAlias(){
}

cAlias::~cAlias(){
}

iFigment::dMigrationPattern
cAlias::getLoadPattern(){
	dMigrationPattern pattern;
	dVersionPlugs version1;

	version1.push_back(&mBound);
	version1.push_back(&mAName);

	pattern.push_back(version1);
	return pattern;
}

const dPlaChar*
cAlias::name() const{
	return mAName.getConst().c_str();
}

dNameHash
cAlias::hash() const{
	return mAHash;
}

void
cAlias::loadEat(cByteBuffer* pBuff, dReloadMap *aReloads){
	cFigment::loadEat(pBuff, aReloads);
	mAHash = makeHash(mAName.get().c_str());
}

void
cAlias::getLinks(std::list<ptrFig>* pOutLinks){
	pOutLinks->push_back(mBound.get());
}

void
cAlias::work(cContext* pCon){
	if(mBound.get().valid())
		mBound.get()->work(pCon);
}

void
cAlias::jack(ptrLead pLead, cContext* pCon){
	if(mBound.get().valid())
		mBound.get()->jack(pLead, pCon);
}

void
cAlias::patBindFig(ptrLead aLead){
	aLead->copyPlug(&mBound, xPT_fig);
}

void
cAlias::patSetAlias(ptrLead aLead){
	aLead->copyPlug(&mAName, xPT_alias);
	mAHash = makeHash(mAName.get().c_str());
}


#ifdef GTUT

#	include "gt_base/unitTestFigments.hpp"

GTUT_START(testAlias, test_suit){
	figmentTestSuit<cAlias>();
}GTUT_END;

#endif
