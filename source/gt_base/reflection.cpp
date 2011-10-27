#include "reflection.hpp"


////////////////////////////////////////////////////////////
using namespace gt;

const cPlugTag *cPlugHound::xPT_contextTargetID = tOutline<cPlugHound>::makePlugTag("context target");
const cPlugTag *cPlugHound::xPT_command = tOutline<cPlugHound>::makePlugTag("command");
const cPlugTag *cPlugHound::xPT_tag = tOutline<cPlugHound>::makePlugTag("tag");
const cPlugTag *cPlugHound::xPT_plug = tOutline<cPlugHound>::makePlugTag("plug");

const cCommand::dUID cPlugHound::xSetup = tOutline<cPlugHound>::makeCommand(
	"setup", &cPlugHound::patSetup,
	cPlugHound::xPT_contextTargetID,
	cPlugHound::xPT_command,
	cPlugHound::xPT_tag,
	NULL
);

const cCommand::dUID cPlugHound::xGetPlug = tOutline<cPlugHound>::makeCommand(
	"get plug", &cPlugHound::patGetPlug,
	cPlugHound::xPT_plug,
	NULL
);

cPlugHound::cPlugHound(){

}

cPlugHound::~cPlugHound(){
}

void
cPlugHound::patSetup(ptrLead aLead){
	mCom = aLead->getPlug(xPT_command);
	mTag = aLead->getPlug(xPT_tag);
	mTarget = aLead->getPlug(xPT_contextTargetID);
	tmpTag = gWorld.get()->getPlugTag(mTag.mD);
}

void
cPlugHound::patGetPlug(ptrLead aLead){
	tmpLead = gWorld.get()->makeLead(mCom.mD, currentCon->getSig());
	ptrFig fig = currentCon->getFirstOfType(mTarget.mD);
	fig->jack(tmpLead, currentCon);
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

	cPlugTag::dUID tagID = *aLead->getPlug(xPT_tag)->exposePtr<cPlugTag::dUID>();
	cPlugTag const *tag = gWorld.get()->getPlugTag(tagID);
	mActualPlugs.push_back( sActualPlug(
		aLead->getPlug(xPT_plug), tag
	) );

	mNewPlugsToAdd.push_back( mNewPlugsToAdd.back() );
}

void cAlucard::patAddConxPlug(ptrLead aLead){
	PROFILE;

	cPlugTag::dUID tagID = *aLead->getPlug(xPT_tag)->exposePtr<cPlugTag::dUID>();
	cPlugTag const *tag = gWorld.get()->getPlugTag(tagID);
	dNameHash fig = *aLead->getPlug(xPT_contextFig)->exposePtr<dNameHash>();
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
			tPlug<ptrFig> found = aCon->getFirstOfType( (*mNewPlugsToFind.begin())->type );

			if( found.mD->hash() != getHash<cEmptyFig>() ){
				mLead->addPlug( &found, (*mNewPlugsToFind.begin())->tag );
			}
			mContextPlugs.pop_front();
		}

		while(!mNewPlugsToAdd.empty()){
			mLead->addPlug(
				&(*mNewPlugsToAdd.begin())->plug, (*mNewPlugsToAdd.begin())->tag
			);
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
	ptrLead setupHound = gWorld.get()->makeLead(cPlugHound::xSetup, fakeConx.getSig());
	ptrLead useHound = gWorld.get()->makeLead(cPlugHound::xGetPlug, fakeConx.getSig());
	tPlug<dNameHash> targetType = getHash<cTestNum>();
	tPlug<cCommand::dUID> comID = cTestNum::xGetData;
	tPlug<cPlugTag::dUID> tagID = cTestNum::xPT_num->mID;

	setupHound->addPlug(&targetType, cPlugHound::xPT_contextTargetID);
	setupHound->addPlug(&comID, cPlugHound::xPT_tag);
	setupHound->addPlug(&tagID, cPlugHound::xPT_tag);
	hound->jack(setupHound, &fakeConx);

	testNum->start(&fakeConx);
	hound->jack(useHound, &fakeConx);
	GTUT_ASRT( *useHound->getPlug(cPlugHound::xPT_plug)->exposePtr<int>() == 42, "Didn't get the right number back from the test figment.");
	testNum->stop(&fakeConx);

}GTUT_END;

GTUT_START(test_reflection, getFromContext){

}GTUT_END;

#endif
