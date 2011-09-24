#include "postman.hpp"

////////////////////////////////////////////////////////////
using namespace gt;

const cPlugTag* cPostman::xPT_target = tOutline<cPostman>::makePlugTag("target");
const cPlugTag* cPostman::xPT_lead = tOutline<cPostman>::makePlugTag("lead");
const cCommand::dUID cPostman::xSetupPostman = tOutline<cPostman>::makeCommand(
	"setup postman",
	&cPostman::patSetup,
	cPostman::xPT_target,
	cPostman::xPT_lead,
	NULL
);

cPostman::cPostman(){
}

cPostman::~cPostman(){
}

void
cPostman::run(cContext* pCon){
	PROFILE;
	start(pCon);
	cContext jackCon;
	mTarget.mD->jack(mLead.mD, &jackCon);
	stop(pCon);
}

void
cPostman::patSetup(cLead *aLead){
	mLead = aLead->getPlug(xPT_target, currentCon);
	mTarget = aLead->getPlug(xPT_lead, currentCon);
}
