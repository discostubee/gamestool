#include "openGL_layer.hpp"

using namespace gt;

void cLayer_X11GL::setDim(dUnitPix pX, dUnitPix pY, dUnitPix pW, dUnitPix pH){
	mHeight = pH;
	mWidth = pW;

    if (mHeight.mD == 0.0f)	// prevent divide-by-zero
        mHeight = 1.0f;
}

cLayer_X11GL::cLayer_X11GL():
	mX(0.0f), mY(0.0f), mWidth(100.0f), mHeight(100.0f)
{
}

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

	// Refresh settings in case something has changed them.
	glShadeModel(GL_SMOOTH);
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClearDepth(1.0f);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
	glViewport(0, 0, mWidth.mD, mHeight.mD);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(45.0f, (mWidth.mD / mHeight.mD), 0.1f, 100.0f);
	glMatrixMode(GL_MODELVIEW);

	mRoot.mD->run(pCon);
	stop(pCon);
}
