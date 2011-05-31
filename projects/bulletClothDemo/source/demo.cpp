#include "demo.hpp"

using namespace gt;

cBulletDemo::cBulletDemo(){

}

cBulletDemo::~cBulletDemo(){

}

void
cBulletDemo::run(cContext* pCon){
	GLfloat mat_specular[] = { 1.0, 1.0, 1.0, 1.0 };
	GLfloat mat_shininess[] = { 50.0 };
	GLfloat light_position[] = { 1.0, 1.0, 1.0, 0.0 };

	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glEnable(GL_COLOR_MATERIAL);
	glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
	glMaterialfv(GL_FRONT, GL_SHININESS, mat_shininess);
	glLightfv(GL_LIGHT0, GL_POSITION, light_position);
	cube();
	ground();
}

void
cBulletDemo::cube(){

	static GLfloat rotate;
	glPushMatrix();
		glTranslatef(0.0f, 0.0f, -7.0f);
		glRotatef(rotate, 1.0f, 0.5f, 0.25f);
		glScalef(0.5f, 0.5f, 0.5f);

		glBegin(GL_QUADS);
			// top of cube
			glNormal3f(0, 1.0f, 0);
			glVertex3f(1.0f, 1.0f, -1.0f);
			glVertex3f(-1.0f, 1.0f, -1.0f);
			glVertex3f(-1.0f, 1.0f, 1.0f);
			glVertex3f(1.0f, 1.0f, 1.0f);
			// bottom of cube
			glNormal3f(0, -1.0f, 0);
			glVertex3f(1.0f, -1.0f, 1.0f);
			glVertex3f(-1.0f, -1.0f, 1.0f);
			glVertex3f(-1.0f, -1.0f, -1.0f);
			glVertex3f(1.0f, -1.0f, -1.0f);
			// front of cube
			glNormal3f(1.0f, 0, 0);
			glVertex3f(1.0f, 1.0f, 1.0f);
			glVertex3f(-1.0f, 1.0f, 1.0f);
			glVertex3f(-1.0f, -1.0f, 1.0f);
			glVertex3f(1.0f, -1.0f, 1.0f);
			// back of cube
			glNormal3f(-1.0f, 0, 0);
			glVertex3f(-1.0f, 1.0f, -1.0f);
			glVertex3f(1.0f, 1.0f, -1.0f);
			glVertex3f(1.0f, -1.0f, -1.0f);
			glVertex3f(-1.0f, -1.0f, -1.0f);
			// right side of cube
			glNormal3f(0, 1.0f, 0);
			glVertex3f(1.0f, 1.0f, -1.0f);
			glVertex3f(1.0f, 1.0f, 1.0f);
			glVertex3f(1.0f, -1.0f, 1.0f);
			glVertex3f(1.0f, -1.0f, -1.0f);
			// left side of cube
			glNormal3f(0, -1.0f, 0);
			glVertex3f(-1.0f, 1.0f, 1.0f);
			glVertex3f(-1.0f, 1.0f, -1.0f);
			glVertex3f(-1.0f, -1.0f, -1.0f);
			glVertex3f(-1.0f, -1.0f, 1.0f);
		glEnd();
	glPopMatrix();
	++rotate;
}

void
cBulletDemo::ground(){

	glPushMatrix();
		glTranslatef(0.0f, -1.0f, 0.0f);
		glBegin(GL_QUADS);
			glNormal3f(0.0f, 1.0f, 0.0f);
			glVertex3f(1.0f, 0.0f, -1.0f);
			glVertex3f(-1.0f, 0.0f, -1.0f);
			glVertex3f(-1.0f, 0.0f, 1.0f);
			glVertex3f(1.0f, 0.0f, 1.0f);
		glEnd();
	glPopMatrix();
}
