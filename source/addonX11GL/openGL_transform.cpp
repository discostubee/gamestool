#include "openGL_transform.hpp"

using namespace gt;

cTransform_X11GL::cTransform_X11GL(){
}

cTransform_X11GL::~cTransform_X11GL(){
}

void cTransform_X11GL::getTransforms(){
}

void cTransform_X11GL::setTransforms(){
}

void cTransform_X11GL::work(cContext *pCon){
	if(!mLink.get().valid())
		return;

	glPushMatrix();

	static GLfloat rotate = 0.0f;
	rotate += 1.0f;
	glTranslatef(0.0f, 0.0f, -7.0f);
	glRotatef(rotate, 1.0f, 0.5f, 0.25f);
	glScalef(1.0f, 1.0f, 1.0f);
	glColor3f(1.0f, 1.0f, 0.0f);

	mLink.get()->work(pCon);
	glPopMatrix();
}
