#include "thread.hpp"

using namespace gt;

const tPlugTag*	cThread::xPT_fig = tOutline<cThread>::makePlugTag("figment");

const cCommand* cThread::xLinkFig = tOutline<cThread>::makeCommand(
	"link fig",
	cThread::eLinkFig,
	cThread::xPT_fig,
	NULL
);

void
cThread::runThread(tPlug<ptrFig> fig, cContext* pCon){
	fig.mD->run(pCon);
}

cThread::cThread(){
}

cThread::~cThread(){
	myThread.join();
}

void
cThread::run(cContext* pCon){
	PROFILE;

	cContext newContext(*pCon);

	pCon->add(this);

	myThread = boost::thread(cThread::runThread, link, &newContext);
}

void
cThread::jack(ptrLead pLead, cContext* pCon){
	PROFILE;

	try{
		switch( pLead->mCom->getSwitch<cThread>() ){
			case eLinkFig:
				break;

			default:
				stop(pCon);
				cFigment::jack(pLead, pCon);
				break;
		}
	}catch(excep::base_error &e){
		WARN(e);
	}catch(...){
		UNKNOWN_ERROR;
	}
}
