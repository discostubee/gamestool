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

const cCommand::dUID cPlugHound::xGimmie = tOutline<cPlugHound>::makeCommand(
	"gimmie", &cPlugHound::patGimmie,
	cPlugHound::xPT_plug,
	NULL
);

cPlugHound::cPlugHound():
		tmpTag(NULL), setup(false)
{}

cPlugHound::~cPlugHound(){
}

void
cPlugHound::patGoGetit(ptrLead aLead){
	PROFILE;

	mCom = aLead->getPlug(xPT_command);
	mTag = aLead->getPlug(xPT_tag);
	mTarget = aLead->getPlug(xPT_contextTargetID);
	tmpTag = gWorld.get()->getPlugTag(mTag.mD);
	tmpLead = gWorld.get()->makeLead(mCom.mD, currentCon->getSig());
	ptrFig fig = currentCon->getFirstOfType(mTarget.mD);
	if(fig->hash() != getHash<cEmptyFig>()){
		currentCon->addJackJob( tmpLead, fig );
	}
}

void
cPlugHound::patGimmie(ptrLead aLead){
	if(tmpLead.get()==NULL)
		throw excep::isNull(__FILE__, __LINE__);

	aLead->addPlug(tmpLead->getPlug(tmpTag), xPT_plug);
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
{}

cAlucard::~cAlucard(){

}

void
cAlucard::patAddCom(ptrLead aLead){
	mCommand = aLead->getPlug(xPT_command);
}

void cAlucard::patSetTarget(ptrLead aLead){
	mTarget = aLead->getPlug(xPT_target);
}

void cAlucard::patAddPlug(ptrLead aLead){
	PROFILE;

	cPlugTag::dUID tagID;
	aLead->getPlug(xPT_tag)->copyInto(&tagID);
	cPlugTag const *tag = gWorld.get()->getPlugTag(tagID);
	mActualPlugs.push_back( sActualPlug(
		aLead->getPlug(xPT_plug), tag
	) );

	mNewPlugsToAdd.push_back( mNewPlugsToAdd.back() );
}

void cAlucard::patAddConxPlug(ptrLead aLead){
	PROFILE;

	cPlugTag::dUID tagID;
	dNameHash fig;

	aLead->getPlug(xPT_tag)->copyInto(&tagID);
	aLead->getPlug(xPT_contextFig)->copyInto(&fig);
	cPlugTag const *tag = gWorld.get()->getPlugTag(tagID);

	mContextPlugs.push_back( sContextPlug(fig, tag) );

	mNewPlugsToFind.push_back( mNewPlugsToFind.back() );
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

	if(mLead.get() == NULL && mCommand.mD != cCommand::noID){
		mLead = ptrLead(new cLead(mCommand.mD, mConx.getSig()) );
	}

	if(mLead.get() != NULL){

		while(!mNewPlugsToFind.empty()){
			tmpCPlug = *mNewPlugsToFind.begin();
			tmpCPlug->found = aCon->getFirstOfType( tmpCPlug->type );

			if( tmpCPlug->found.mD->hash() != getHash<cEmptyFig>() ){
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

		if(mTarget.mD->hash() != getHash<cEmptyFig>()){
			mTarget.mD->jack(mLead, &mConx);
		}
	}

	stop(aCon);
}

#ifdef GTUT
	#include "unitTestFigments.hpp"

GTUT_START(test_reflection, houndGets){
	tOutline<cPlugHound>::draft();
	tOutline<cTestNum>::draft();

	cContext fakeConx;
	ptrFig testNum = gWorld.get()->makeFig(getHash<cTestNum>());
	ptrFig hound = gWorld.get()->makeFig(getHash<cPlugHound>());
	ptrLead sendHound = gWorld.get()->makeLead(cPlugHound::xGoGetIt, fakeConx.getSig());
	ptrLead getFromHound = gWorld.get()->makeLead(cPlugHound::xGimmie, fakeConx.getSig());
	tPlug<dNameHash> targetType = getHash<cTestNum>();
	tPlug<cCommand::dUID> comID = cTestNum::xGetData;
	tPlug<cPlugTag::dUID> tagID = cTestNum::xPT_num->mID;

	sendHound->addPlug(&targetType, cPlugHound::xPT_contextTargetID);
	sendHound->addPlug(&comID, cPlugHound::xPT_command);
	sendHound->addPlug(&tagID, cPlugHound::xPT_tag);

	testNum->start(&fakeConx);
	hound->jack(sendHound, &fakeConx);
	testNum->stop(&fakeConx);
	fakeConx.runJackJobs();

	hound->jack(getFromHound, &fakeConx);
	//GTUT_ASRT( *getFromHound->getPlug(cPlugHound::xPT_plug)->exposePtr<int>() == 42, "Didn't get the right number back from the test figment.");

}GTUT_END;

GTUT_START(test_reflection, alucardBasic){
	tOutline<cAlucard>::draft();
	tOutline<cTestNum>::draft();

	cContext fakeConx;
	ptrFig alucard = gWorld.get()->makeFig(getHash<cAlucard>());
	tPlug<ptrFig> testNum = gWorld.get()->makeFig(getHash<cTestNum>());

	ptrLead setCom = gWorld.get()->makeLead(cAlucard::xAddCommand, fakeConx.getSig());
	tPlug<cCommand::dUID> comID = cTestNum::xSetData;
	setCom->addPlug(&comID, cAlucard::xPT_command);
	alucard->jack(setCom, &fakeConx);

	tPlug<int> num = 3;
	ptrLead setPlug = gWorld.get()->makeLead(cAlucard::xAddPlug, fakeConx.getSig());
	setPlug->addPlug(&num, cAlucard::xPT_plug);
	alucard->jack(setPlug, &fakeConx);

	ptrLead setTarget = gWorld.get()->makeLead(cAlucard::xSetTarget, fakeConx.getSig());
	setTarget->addPlug(&testNum, cAlucard::xPT_target);
	alucard->jack(setTarget, &fakeConx);

	alucard->run(&fakeConx);

	ptrLead getNum = gWorld.get()->makeLead( cTestNum::xGetData, fakeConx.getSig() );
	testNum.mD->jack(getNum, &fakeConx);
	GTUT_ASRT( *getNum->getPlug(cTestNum::xPT_num) == num, "number wasn't set.");


}GTUT_END;

#endif
