#include "postman.hpp"

////////////////////////////////////////////////////////////
using namespace gt;

const tPlugTag* cPostman::xPT_target = tOutline<cPostman>::makePlugTag("target");
const tPlugTag* cPostman::xPT_lead = tOutline<cPostman>::makePlugTag("lead");
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
}

void cPostman::jack(ptrLead cLead, cContext* pCon){
}
