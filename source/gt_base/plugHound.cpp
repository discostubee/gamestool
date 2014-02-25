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

#include "plugHound.hpp"


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

	version1.push_back(&mCom);
	version1.push_back(&mTag);
	version1.push_back(&mTarget);

	pattern.push_back(version1);
	return pattern;
}

void
cPlugHound::patGoGetit(ptrLead aLead){
	PROFILE;

	aLead->copyPlug(&mCom, xPT_command);
	aLead->copyPlug(&mTag, xPT_tag);
	aLead->copyPlug(&mTarget, xPT_contextTargetID);

	ptrLead getLead = gWorld.get()->makeLead(mCom.get());
	ptrFig fig = currentCon->getFirstOfType(mTarget.get());

	const cPlugTag *tagTarget = gWorld.get()->getPlugTag(fig->hash(), mTag.get());

	fig->jack(getLead, currentCon);
	getLead->passPlug(
		aLead.get(),
		tagTarget,
		xPT_plug
	);
}

#ifdef GTUT

#include "unitTestFigments.hpp"

GTUT_START(test_cPlugHound, test_suit){
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

	sendHound->linkPlug(&targetType, cPlugHound::xPT_contextTargetID);
	sendHound->linkPlug(&comID, cPlugHound::xPT_command);
	sendHound->linkPlug(&tagID, cPlugHound::xPT_tag);

	tPlug<int> num;

	testNum->start(&fakeConx);
	hound->jack(sendHound, &fakeConx);
	testNum->stop(&fakeConx);

	startLead(sendHound, fakeConx.getSig());
	sendHound->copyPlug(&num, cPlugHound::xPT_plug);
	stopLead(sendHound);

	GTUT_ASRT( num.get() == 42, "Didn't get the right number back from the test figment.");
}GTUT_END;
#endif
