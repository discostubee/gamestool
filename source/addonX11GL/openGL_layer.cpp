#include "openGL_layer.hpp"

using namespace gt;

cLayer_X11GL::cLayer_X11GL()
{}

cLayer_X11GL::~cLayer_X11GL(){
}

void
cLayer_X11GL::run(cContext* pCon){
	start(pCon);
	mLink.mD->run(pCon);
	stop(pCon);
}
