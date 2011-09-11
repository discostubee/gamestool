#include "openGL_layer.hpp"

using namespace gt;

cLayer_X11GL::cLayer_X11GL()
{}

cLayer_X11GL::~cLayer_X11GL(){
}

void
cLayer_X11GL::run(cContext* pCon){
	start(pCon);
	try{
		(void)pCon->getLastOfType(getHash<cWindowFrame>());
	}catch(...){
		DBUG_LO("Layer can't work without a window frame.");
		return;
	}

	mLink.mD->run(pCon);
	stop(pCon);
}
