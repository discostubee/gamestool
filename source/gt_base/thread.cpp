#include "thread.hpp"

using namespace gt;

const cPlugTag*	cThread::xPT_fig = tOutline<cThread>::makePlugTag("figment");

const cCommand* cThread::xLinkFig = tOutline<cThread>::makeCommand(
	"link fig",
	cThread::eLinkFig,
	cThread::xPT_fig,
	NULL
);

cThread::cThread(){
}

cThread::~cThread(){
}

void
cThread::run(cContext* pCon){
	PROFILE;

	cContext newContext(*pCon);

	pCon->add(this);
	link.mD->run(&newContext);
}

void
cThread::jack(ptrLead pLead, cContext* pCon){
	PROFILE;

	try{
		switch( pLead->mCom->getSwitch<cThread>() ){
			case eLinkFig:
				break;

			default:
				cFigment::jack(pLead, pCon);
				break;
		}
	}catch(excep::base_error &e){
		WARN(e);
	}catch(...){
		UNKNOWN_ERROR;
	}
}
