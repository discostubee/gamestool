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

#include "reflection.hpp"


////////////////////////////////////////////////////////////
using namespace gt;

const cPlugTag *cPlugHound::xPT_contextTargetID = tOutline<cPlugHound>::makePlugTag("context target");
const cPlugTag *cPlugHound::xPT_command = tOutline<cPlugHound>::makePlugTag("command");
const cPlugTag *cPlugHound::xPT_tag = tOutline<cPlugHound>::makePlugTag("tag");
const cPlugTag *cPlugHound::xPT_plug = tOutline<cPlugHound>::makePlugTag("plug");

const cCommand::dUID cPlugHound::xGoGetIt = tOutline<cPlugHound>::makeCommand(
	"go get it", &cPlugHound::patGoGetit,
	cPlugHound::xPT_contextTargetID,
	cPlugHound::xPT_command,
	cPlugHound::xPT_tag,
	NULL
);


cPlugHound::cPlugHound(){
	addUpdRoster(&mCom);
	addUpdRoster(&mTag);
	addUpdRoster(&mTarget);
}

cPlugHound::~cPlugHound(){
}

cFigment::dMigrationPattern
cPlugHound::getLoadPattern(){
	dMigrationPattern pattern;
	dVersionPlugs version1;

	version1.push_back(mCom);
	version1.push_back(mTag);
	version1.push_back(mTarget);

	pattern.push_back(version1);
	return pattern;
}

void
cPlugHound::patGoGetit(ptrLead aLead){
	PROFILE;

	aLead->getPlug(&mCom, xPT_command);
	aLead->getPlug(&mTag, xPT_tag);
	aLead->getPlug(&mTarget, xPT_contextTargetID);

	ptrLead getLead = gWorld.get()->makeLead(mCom.get());
	ptrFig fig = currentCon->getFirstOfType(mTarget.get());

	const cPlugTag *tagTarget = gWorld.get()->getPlugTag(mTag.get());

	fig->jack(getLead, currentCon);
	getLead->passPlug(
		aLead.get(),
		tagTarget,
		xPT_plug
	);
}

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

	version1.push_back(mBound);
	version1.push_back(mAName);

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
	aLead->getPlug(&mBound, xPT_fig);
}

void
cAlias::patSetAlias(ptrLead aLead){
	aLead->getPlug(&mAName, xPT_alias);
	mAHash = makeHash(mAName.get().c_str());
}

/*
////////////////////////////////////////////////////////////
using namespace gt;

const cPlugTag *cAlucard::xPT_figHash = tOutline<cAlucard>::makePlugTag("fig hash");
const cPlugTag *cAlucard::xPT_comUID = tOutline<cAlucard>::makePlugTag("command ID");
const cPlugTag *cAlucard::xPT_target = tOutline<cAlucard>::makePlugTag("target");
const cPlugTag *cAlucard::xPT_plug = tOutline<cAlucard>::makePlugTag("plug");
const cPlugTag *cAlucard::xPT_tag = tOutline<cAlucard>::makePlugTag("tag");

const cCommand::dUID cAlucard::xSetCommand = tOutline<cAlucard>::makeCommand(
	"set command", &cAlucard::patSetCom, xPT_comUID, NULL
);

const cCommand::dUID cAlucard::xSetTarget = tOutline<cAlucard>::makeCommand(
	"set target", &cAlucard::patSetTarget, xPT_target, NULL
);

const cCommand::dUID cAlucard::xAddPlug = tOutline<cAlucard>::makeCommand(
	"add plug", &cAlucard::patAddPlug, xPT_plug, xPT_tag, NULL
);

const cCommand::dUID cAlucard::xSetTargetConx = tOutline<cAlucard>::makeCommand(
	"set context target", &cAlucard::patAddPlugConx, xPT_figHash, NULL
);

const cCommand::dUID cAlucard::xAddPlugConx = tOutline<cAlucard>::makeCommand(
	"add context plug", &cAlucard::patAddPlugConx, xPT_figHash, xPT_tag, NULL
);


cAlucard::cAlucard():
	mCommand(cCommand::noID)
{
	addUpdRoster(&mTarget);
	addUpdRoster(&mAltTargetName);
	addUpdRoster(&mCommand);
}

cAlucard::~cAlucard(){
}


void cAlucard::getLinks(std::list<ptrFig>* pOutLinks){
}

void cAlucard::work(cContext* aCon){
	PROFILE;

}
*/




#ifdef GTUT

#include "unitTestFigments.hpp"

GTUT_START(test_cPlugHound, test_suit){
	tOutline<cFigment>::draft();
	figmentTestSuit<cPlugHound>();
}GTUT_END;


GTUT_START(test_cPlugHound, houndGets){
	tOutline<cPlugHound>::draft();
	tOutline<cTestNum>::draft();

	cContext fakeConx;

	ptrFig testNum = gWorld.get()->makeFig(getHash<cTestNum>());
	ptrFig hound = gWorld.get()->makeFig(getHash<cPlugHound>());
	ptrLead sendHound = gWorld.get()->makeLead(cPlugHound::xGoGetIt);
	tPlug<dNameHash> targetType = getHash<cTestNum>();
	tPlug<cCommand::dUID> comID = cTestNum::xGetData;
	tPlug<cPlugTag::dUID> tagID = cTestNum::xPT_num->mID;

	sendHound->addPlug(&targetType, cPlugHound::xPT_contextTargetID);
	sendHound->addPlug(&comID, cPlugHound::xPT_command);
	sendHound->addPlug(&tagID, cPlugHound::xPT_tag);

	testNum->start(&fakeConx);
	hound->jack(sendHound, &fakeConx);
	testNum->stop(&fakeConx);

	tPlug<int> num;

	startLead(sendHound, fakeConx.getSig());
	sendHound->getPlug(&num, cPlugHound::xPT_plug);
	stopLead(sendHound);

	GTUT_ASRT( num.get() == 42, "Didn't get the right number back from the test figment.");
}GTUT_END;

/*
GTUT_START(test_cAlucard, test_suit){
	tOutline<cFigment>::draft();
	figmentTestSuit<cAlucard>();
}GTUT_END;

GTUT_START(test_reflection, alucardBasic){


}GTUT_END;
*/
#endif
