#include "openGL_polyMesh.hpp"

using namespace gt;

cPolyMesh_GL::cPolyMesh_GL():
		vbuff(NULL), ibuff(NULL), mVBO(0), mIBO(0), polyCount(0), vertCount(0)
{}

cPolyMesh_GL::~cPolyMesh_GL(){
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glDeleteBuffers(1, &mIBO);
	glDeleteBuffers(1, &mVBO);
	delete [] vbuff;
	delete [] ibuff;
}

void
cPolyMesh_GL::run(cContext *pCon){
	PROFILE;

	start(pCon);

	if(mLazyMesh)
		formatGLMesh();

	glBindBuffer(GL_ARRAY_BUFFER, mVBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mIBO);

	glPushMatrix();

		//!!! test code.
		static GLfloat rotate = 0.0f;
		rotate += 1.0f;
		glTranslatef(0.0f, 0.0f, -7.0f);
		glRotatef(rotate, 1.0f, 0.5f, 0.25f);
		glScalef(1.0f, 1.0f, 1.0f);
		glColor3f(1.0f, 1.0f, 0.0f);
		//!!!

		glDrawArrays(GL_TRIANGLES, 0, vertCount);	//glDrawElements(GL_TRIANGLES, polyCount, GL_UNSIGNED_INT, BUFFER_OFFSET(0));

	glPopMatrix();

	stop(pCon);
}

void
cPolyMesh_GL::formatGLMesh(){
	PROFILE;

	if(mLazyMesh->mVertexes.empty())
		THROW_BASEERROR("lazy mesh has no vertexes");

	if(mLazyMesh->mPolys.empty())
		THROW_BASEERROR("lazy mesh has no polies");

	polyCount = mLazyMesh->mPolys.size();
	vertCount = polyCount * 3;

	if(mIBO){
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
		glDeleteBuffers(1, &mIBO);
		delete [] ibuff;
	}
	if(mVBO){
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glDeleteBuffers(1, &mVBO);
		delete [] vbuff;
	}

	vbuff = new sGLFloatVert[vertCount];
	ibuff = new dIdxV[polyCount];

	size_t lazyVertCnt = mLazyMesh->mVertexes.size();

	try{
		sPoly *tempPoly;
		sVertex *tempVert;
		size_t idxV, idxABC;
		sGLFloatVert *itrVBuff = vbuff;

		for(dIdxV idxPoly = 0; idxPoly < polyCount; ++idxPoly){
			tempPoly = &mLazyMesh->mPolys[idxPoly];

			for(idxV = 0; idxV < 3; ++idxV){
				switch(idxV){
					case 0: idxABC = tempPoly->a; break;
					case 1: idxABC = tempPoly->b; break;
					case 2: idxABC = tempPoly->c; break;
				}

				if(idxABC >= lazyVertCnt)
					throw excep::base_error("bad index to vertex array", __FILE__, __LINE__);

				tempVert = &mLazyMesh->mVertexes[idxABC];

				itrVBuff->x = tempVert->x;
				itrVBuff->y = tempVert->y;
				itrVBuff->z = tempVert->z;
				//!\todo	assign normals.
				//!\todo	assign mapping.

				++itrVBuff;

			}
			ibuff[idxPoly] = idxPoly;
		}

		glGenBuffers(1, &mVBO);		// Generate 1 vertex array.
		glBindBuffer(GL_ARRAY_BUFFER, mVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(sGLFloatVert)*vertCount, vbuff, GL_STATIC_DRAW);

		glGenBuffers(1, &mIBO);		// Generate 1 index buffer
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mIBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(dIdxV)*polyCount, ibuff, GL_STATIC_DRAW);

		//- We can assume this will always be the same for every buffer.
		glVertexPointer(3, GL_FLOAT, sizeof(sGLFloatVert), BUFFER_OFFSET(0));
		//glNormalPointer(GL_FLOAT, 64, BUFFER_OFFSET(12));
		//glClientActiveTexture(GL_TEXTURE0);
		//glTexCoordPointer(2, GL_FLOAT, 64, BUFFER_OFFSET(24));
		//glClientActiveTexture(GL_TEXTURE1);
		//glTexCoordPointer(2, GL_FLOAT, 64, BUFFER_OFFSET(32));
		//glClientActiveTexture(GL_TEXTURE2);
		//glTexCoordPointer(2, GL_FLOAT, 64, BUFFER_OFFSET(40));

	}catch(excep::base_error &e){
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
		glDeleteBuffers(1, &mIBO);
		delete [] ibuff;

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glDeleteBuffers(1, &mVBO);
		delete [] vbuff;

		WARN(e);
	}

	cleanLazy();
}
