#include "openGL_polyMesh.hpp"

using namespace gt;

cPolyMesh_X11GL::cPolyMesh_X11GL()
: mVBO(0), mIBO(0), polyCount(0), vertCount(0), mCache(HASH_INVALID)
{}

cPolyMesh_X11GL::~cPolyMesh_X11GL(){
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glDeleteBuffers(1, &mIBO);
	glDeleteBuffers(1, &mVBO);
}

void
cPolyMesh_X11GL::work(cContext *pCon){
	PROFILE;

	if(mUpdateLazy){
		formatGLMesh();
		mUpdateLazy = false;
	}

	glBindBuffer(GL_ARRAY_BUFFER, mVBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mIBO);
	glDrawElements(GL_TRIANGLES, polyCount*DIMENSIONS, GL_UNSIGNED_INT, 0);
}

void
cPolyMesh_X11GL::downloadLazy(){
	if(!mUpdateLazy && mCache != HASH_INVALID){
		cleanLazy();
		dRefWorld w = gWorld.get();
		sMesh &ref = mLazyMesh.get();
		tAutoPtr<cArray> coldVerts = w->getFridge()->thaw(mCache);
		mCache = mCache << 1;
		tAutoPtr<cArray> coldPolys = w->getFridge()->thaw(mCache);
		mCache = mCache >> 1;

		if(!coldVerts.isValid() || !coldPolys.isValid())
			THROW_ERROR("Couldn't thaw lazy mesh.");

		for(size_t i=0; i <= (coldVerts->mLen / sizeof(dGLFloat)) + DIMENSIONS; i += DIMENSIONS){
			ref.mVertexes.push_back(
				sVertex(
					static_cast<dGLFloat>(coldVerts->mData[ i]),
					static_cast<dGLFloat>(coldVerts->mData[ i+sizeof(dGLFloat) ]),
					static_cast<dGLFloat>(coldVerts->mData[ i+(2*sizeof(dGLFloat)) ])
				)
			);
		}

		for(size_t i=0; i < (coldPolys->mLen / sizeof(dIdxV)) + DIMENSIONS; i += DIMENSIONS){
			ref.mPolys.push_back(
				sPoly(
					static_cast<dIdxVert>(coldPolys->mData[ i ]),
					static_cast<dIdxVert>(coldPolys->mData[ i+sizeof(dIdxVert) ]),
					static_cast<dIdxVert>(coldPolys->mData[ i+(2*sizeof(dIdxVert)) ])
				)
			);
		}
	}
}

dNameHash
cPolyMesh_X11GL::makeFridgeID(dGLFloat *buffVert, size_t lenVerts, dIdxV * buffPoly, size_t lenPolys){
	dNameHash rtn=0;
	for(size_t i=0; i<lenVerts; ++i){
		rtn = (rtn << sizeof(dGLFloat))
			^ reinterpret_cast<dNameHash*>(&buffVert[i])[0]
			^ reinterpret_cast<dNameHash*>(&buffVert[i])[1]
		;
	}

	for(size_t i=0; i<lenPolys; ++i)
		rtn = (rtn << sizeof(dIdxV)) ^ buffPoly[i];

	return rtn;
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
	}
	if(mVBO){
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glDeleteBuffers(1, &mVBO);
	}

	dGLFloat *vbuff = new dGLFloat[vertCount * DIMENSIONS];
	dIdxV *ibuff = new dIdxV[polyCount * DIMENSIONS];

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

	cFridge * f = gWorld.get()->getFridge();
	mCache = makeFridgeID(vbuff, vertCount * DIMENSIONS, ibuff, polyCount * DIMENSIONS);
	f->chill(
		mCache,
		tAutoPtr<cArray>(
			new cArray(reinterpret_cast<dByte*>(vbuff), vertCount * DIMENSIONS * sizeof(dGLFloat))
		)
	);
	mCache = mCache << 1;
	f->chill(
		mCache,
		tAutoPtr<cArray>(
			new cArray(reinterpret_cast<dByte*>(ibuff), polyCount * DIMENSIONS * sizeof(dIdxV))
		)
	);
	mCache = mCache >> 1;

	delete [] vbuff;
	delete [] ibuff;

	cleanLazy();
}

