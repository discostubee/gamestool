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

void
cPlugHound::patGoGetit(ptrLead aLead){
	PROFILE;

	aLead->getPlug(&mCom, xPT_command);
	aLead->getPlug(&mTag, xPT_tag);
	aLead->getPlug(&mTarget, xPT_contextTargetID);

	const cPlugTag *getTag = gWorld.get()->getPlugTag(mTag.get());
	ptrLead getLead = gWorld.get()->makeLead(mCom.get());
	ptrFig fig = currentCon->getFirstOfType(mTarget.get());


}


////////////////////////////////////////////////////////////
using namespace gt;

const cPlugTag *cAlucard::xPT_command = tOutline<cAlucard>::makePlugTag("command");
const cPlugTag *cAlucard::xPT_target = tOutline<cAlucard>::makePlugTag("target");
const cPlugTag *cAlucard::xPT_plug = tOutline<cAlucard>::makePlugTag("plug");
const cPlugTag *cAlucard::xPT_tag = tOutline<cAlucard>::makePlugTag("tag");
const cPlugTag *cAlucard::xPT_contextFig = tOutline<cAlucard>::makePlugTag("context fig");

const cCommand::dUID cAlucard::xAddCommand = tOutline<cAlucard>::makeCommand(
	"add command", &cAlucard::patAddCom, xPT_command, NULL
);

const cCommand::dUID cAlucard::xSetTarget = tOutline<cAlucard>::makeCommand(
	"set target", &cAlucard::patSetTarget, xPT_target, NULL
);

const cCommand::dUID cAlucard::xAddPlug = tOutline<cAlucard>::makeCommand(
	"add plug", &cAlucard::patAddPlug, xPT_plug, xPT_tag, NULL
);

const cCommand::dUID cAlucard::xAddContextPlug = tOutline<cAlucard>::makeCommand(
	"add context plug", &cAlucard::patAddConxPlug, xPT_contextFig, xPT_tag, NULL
);

const cCommand::dUID cAlucard::xSetContextTarget = tOutline<cAlucard>::makeCommand(
	"set context target", &cAlucard::patAddConxPlug, xPT_contextFig, NULL
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

void
cAlucard::patAddCom(ptrLead aLead){
	aLead->getPlug(&mCommand, xPT_command);
}

void cAlucard::patSetTarget(ptrLead aLead){
	aLead->getPlug(&mTarget, xPT_target);
}

void cAlucard::patAddPlug(ptrLead aLead){
	PROFILE;

}

void cAlucard::patAddConxPlug(ptrLead aLead){
	PROFILE;


}

void cAlucard::patSetConxTarget(ptrLead aLead){
}

void cAlucard::save(cByteBuffer* pAddHere){
}

void cAlucard::loadEat(cByteBuffer* pBuff, dReloadMap *aReloads){
}

void cAlucard::getLinks(std::list<ptrFig>* pOutLinks){
}

void cAlucard::run(cContext* aCon){
	PROFILE;
	start(aCon);

	if(mLead.get() == NULL && mCommand.get() != cCommand::noID){
		mLead = ptrLead( new cLead(mCommand.get()) );
	}

	if(mLead.get() != NULL){

		while(!mNewPlugsToFind.empty()){
			tmpCPlug = *mNewPlugsToFind.begin();
			tmpCPlug->found = aCon->getFirstOfType( tmpCPlug->type );

			if( tmpCPlug->found.get()->hash() != getHash<cEmptyFig>() ){
				mLead->addPlug(&tmpCPlug->found, tmpCPlug->tag);
			}
			mContextPlugs.pop_front();
		}

		while(!mNewPlugsToAdd.empty()){
			tmpAPlug = *mNewPlugsToAdd.begin();
			mLead->addPlug(
				&tmpAPlug->plug, tmpAPlug->tag
			);
			mNewPlugsToAdd.pop_front();
		}

		if(mTarget.get()->hash() != getHash<cEmptyFig>()){
			mTarget.get()->jack(mLead, &mConx);
		}
	}

	stop(aCon);
}

#ifdef GTUT

#include "unitTestFigments.hpp"

GTUT_START(test_cPlugHound, test_suit){
	tOutline<cFigment>::draft();
	figmentTestSuit<cPlugHound>();
}GTUT_END;

GTUT_START(test_cAlucard, test_suit){
	tOutline<cFigment>::draft();
	figmentTestSuit<cAlucard>();
}GTUT_END;

GTUT_START(test_reflection, houndGets){
	tOutline<cPlugHound>::draft();
	tOutline<cTestNum>::draft();

	cContext fakeConx;

	ptrFig testNum = gWorld.get()->makeFig(getHash<cTestNum>());
	ptrFig hound = gWorld.get()->makeFig(getHash<cPlugHound>());
	ptrLead sendHound = gWorld.get()->makeLead(cPlugHound::xGoGetIt);
	tPlug<dNameHash> targetType = getHash<cTestNum>();
	tPlug<cCommand::dUID> comID = cTestNum::xGetData;
	tPlug<cPlugTag::dUID> tagID = cTestNum::xPT_num->mID;

	{
		FAUX_JACK(sendHound, fakeConx);
		sendHound->addPlug(&targetType, cPlugHound::xPT_contextTargetID);
		sendHound->addPlug(&comID, cPlugHound::xPT_command);
		sendHound->addPlug(&tagID, cPlugHound::xPT_tag);
	}

	testNum->start(&fakeConx);
	hound->jack(sendHound, &fakeConx);
	testNum->stop(&fakeConx);

	tPlug<int> num;
	sendHound->getPlug(&num, cPlugHound::xPT_plug);
	GTUT_ASRT( num.get() == 42, "Didn't get the right number back from the test figment.");
}GTUT_END;

GTUT_START(test_reflection, alucardBasic){
	tOutline<cAlucard>::draft();
	tOutline<cTestNum>::draft();

	cContext fakeConx;

	ptrFig alucard = gWorld.get()->makeFig(getHash<cAlucard>());
	tPlug<ptrFig> testNum = gWorld.get()->makeFig(getHash<cTestNum>());

	ptrLead setCom = gWorld.get()->makeLead(cAlucard::xAddCommand);
	tPlug<cCommand::dUID> comID = cTestNum::xSetData;
	{
		FAUX_JACK(setCom, fakeConx);
		setCom->addPlug(&comID, cAlucard::xPT_command);
	}
	alucard->jack(setCom, &fakeConx);

	tPlug<int> num = 3;
	ptrLead setPlug = gWorld.get()->makeLead(cAlucard::xAddPlug);
	{
		FAUX_JACK(setCom, fakeConx);
		setPlug->addPlug(&num, cAlucard::xPT_plug);
	}
	alucard->jack(setPlug, &fakeConx);

	ptrLead setTarget = gWorld.get()->makeLead(cAlucard::xSetTarget);
	{
		FAUX_JACK(setTarget, fakeConx);
		setTarget->addPlug(&testNum, cAlucard::xPT_target);
	}
	alucard->jack(setTarget, &fakeConx);

	alucard->run(&fakeConx);

	ptrLead getNum = gWorld.get()->makeLead( cTestNum::xGetData );
	testNum.get()->jack(getNum, &fakeConx);
	{
		FAUX_JACK(getNum, fakeConx);
		tPlug<int> a;
		getNum->getPlug(&a, cTestNum::xPT_num);
		GTUT_ASRT( a == num, "number wasn't set.");
	}

}GTUT_END;

#endif
