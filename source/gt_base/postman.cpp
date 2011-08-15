#include "postman.hpp"

////////////////////////////////////////////////////////////
using namespace gt;

const cPlugTag* cPostman::xPT_target = tOutline<cPostman>::makePlugTag("target");
const cPlugTag* cPostman::xPT_lead = tOutline<cPostman>::makePlugTag("lead");
const cCommand* cPostman::xSetupPostman = tOutline<cPostman>::makeCommand(
	"setup postman",
	cPostman::eSwitchEnd,
	cPostman::xPT_target,
	cPostman::xPT_lead,
	NULL
);

cPostman::cPostman(){
}

cPostman::~cPostman(){
}

void cPostman::run(cContext* pCon){
	PROFILE;
	start(pCon);
	cContext jackCon;
	mTarget.mD->jack(mLead.mD, &jackCon);
	stop(pCon);
}

void cPostman::jack(ptrLead pLead, cContext* pCon){
	PROFILE;
	start(pCon);
	try{
		switch( pLead->mCom->getSwitch<cPostman>() ){
			case eSetup:

				break;

			default:
				stop(pCon);
				cFigment::jack(pLead, pCon);
				break;
		}
	}catch(excep::base_error &e){
		WARN(e);
	}
	stop(pCon);
}
