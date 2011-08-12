#include "openGL_layer.hpp"

using namespace gt;

cLayer_winGL::cLayer_winGL(){
}

cLayer_winGL::~cLayer_winGL(){
}

void 
cLayer_winGL::run(cContext* pCon){
	ASRT_NOTNULL(pCon);

	pCon->add(this);
	mRoot.mD->run(pCon);
	pCon->finished(this);
}