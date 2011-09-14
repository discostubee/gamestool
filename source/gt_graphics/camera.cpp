#include "camera.hpp"

using namespace gt;

const cPlugTag *cCamera::xPT_mode = tOutline<cCamera>::makePlugTag("mode");
const cCommand *cCamera::xSetDMode = tOutline<cCamera>::makeCommand(
	"set mode",
	cCamera::eSetDMode,
	xPT_mode,
	NULL
);

cCamera::cCamera(){
}

cCamera::~cCamera(){
}

void
cCamera::jack(ptrLead pLead, cContext *pCon){
	start(pCon);
	try{
		switch(pLead->mCom->getSwitch<cCamera>()){

			case eSetDMode:{

			}break;

			default:
				cFigment::jack(pLead, pCon);
				break;
		}
	}catch(excep::base_error &e){
		WARN(e);
	}
	stop(pCon);
}
