#include "openGL_polyMesh.hpp"

using namespace gt;

cPolyMesh_X11GL::cPolyMesh_X11GL():
		vbuff(NULL), ibuff(NULL), mVBO(0), mIBO(0), polyCount(0), vertCount(0)
{}

cPolyMesh_X11GL::~cPolyMesh_X11GL(){
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glDeleteBuffers(1, &mIBO);
	glDeleteBuffers(1, &mVBO);
	delete [] vbuff;
	delete [] ibuff;
}

void
cPolyMesh_X11GL::run(cContext *pCon){
	PROFILE;

	start(pCon);

	if(mUpdateLazy){
		formatGLMesh();
		mUpdateLazy = false;
	}

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

	glDrawElements(GL_TRIANGLES, polyCount*DIMENSIONS, GL_UNSIGNED_INT, 0);

	glPopMatrix();

	stop(pCon);
}

void
cPolyMesh_X11GL::formatGLMesh(){
	PROFILE;

	sMesh &ref = mLazyMesh.get();

	if(ref.mVertexes.empty())
		THROW_ERROR("lazy mesh has no vertexes");

	if(ref.mPolys.empty())
		THROW_ERROR("lazy mesh has no polies");

	polyCount = ref.mPolys.size();
	vertCount = ref.mVertexes.size();

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

	vbuff = new dGLFloat[vertCount * DIMENSIONS];
	ibuff = new dIdxV[polyCount * DIMENSIONS];

	try{
		for(size_t iVert=0; iVert < vertCount; ++iVert){
			vbuff[iVert + IOA_X] = static_cast<dGLFloat>( ref.mVertexes[iVert].x );
			vbuff[iVert + IOA_Y] = static_cast<dGLFloat>( ref.mVertexes[iVert].y );
			vbuff[iVert + IOA_Z] = static_cast<dGLFloat>( ref.mVertexes[iVert].z );
		}

		glGenBuffers(1, &mVBO);		// Generate 1 vertex array.
		glBindBuffer(GL_ARRAY_BUFFER, mVBO);
		glEnableVertexAttribArray(0);	//- positions in stream 0.
		glVertexPointer(DIMENSIONS, GL_FLOAT, 0, NULL);
		glBufferData(
			GL_ARRAY_BUFFER,
			sizeof(dGLFloat) * vertCount * DIMENSIONS,
			vbuff,
			GL_STATIC_DRAW
		);

		size_t iPoly=0;
		for(
			std::vector<sPoly>::iterator itrP = ref.mPolys.begin();
			itrP != ref.mPolys.end();
			++itrP
		){
			ASRT_INRANGE(ref.mVertexes, itrP->a);
			ibuff[iPoly++] = static_cast<dIdxV>(itrP->a);

			ASRT_INRANGE(ref.mVertexes, itrP->b);
			ibuff[iPoly++] = static_cast<dIdxV>(itrP->b);

			ASRT_INRANGE(ref.mVertexes, itrP->c);
			ibuff[iPoly++] = static_cast<dIdxV>(itrP->c);
		}

		glGenBuffers(1, &mIBO);		// Generate 1 index buffer
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mIBO);
		glBufferData(
			GL_ELEMENT_ARRAY_BUFFER,
			sizeof(dIdxV) * polyCount * DIMENSIONS,
			ibuff,
			GL_STATIC_DRAW
		);

	}catch(excep::base_error &e){
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
		glDeleteBuffers(1, &mIBO);

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glDeleteBuffers(1, &mVBO);

		WARN(e);
	}

	SAFEDEL_ARR(ibuff);
	SAFEDEL_ARR(vbuff);

	cleanLazy();
}

