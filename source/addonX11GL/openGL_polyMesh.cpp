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

	if(mLazyMesh.get().mFresh){
		formatGLMesh();
		mLazyMesh.get().mFresh = false;
	}

	glBindBuffer(GL_ARRAY_BUFFER, mVBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mIBO);
	glDrawElements(GL_TRIANGLES, polyCount, GL_UNSIGNED_INT, 0);
}

void
cPolyMesh_X11GL::downloadLazy(){
	if(mLazyMesh.get().mFresh == false && mCache != HASH_INVALID){
		cleanLazy();
		dRefWorld w = gWorld.get();
		sMesh &ref = mLazyMesh.get();
		tAutoPtr<iArray> coldVerts = w->getFridge()->thaw(mCache);
		mCache = mCache << 1;
		tAutoPtr<iArray> coldPolys = w->getFridge()->thaw(mCache);
		mCache = mCache >> 1;

		if(!coldVerts.isValid() || !coldPolys.isValid())
			THROW_ERROR("Couldn't thaw lazy mesh.");

		for(size_t i=0; i <= (coldVerts->getSize() / sizeof(dGLFloat)) + DIMENSIONS; i += DIMENSIONS){
			ref.mVertexes.push_back(
				sVertex(
					static_cast<dGLFloat>(coldVerts->getBuff()[ i]),
					static_cast<dGLFloat>(coldVerts->getBuff()[ i+sizeof(dGLFloat) ]),
					static_cast<dGLFloat>(coldVerts->getBuff()[ i+(2*sizeof(dGLFloat)) ])
				)
			);
		}

		for(size_t i=0; i < (coldPolys->getSize() / sizeof(dIdxV)) + DIMENSIONS; i += DIMENSIONS){
			ref.mPolys.push_back(
				sPoly(
					static_cast<dIdxVert>(coldPolys->getBuff()[ i ]),
					static_cast<dIdxVert>(coldPolys->getBuff()[ i+sizeof(dIdxVert) ]),
					static_cast<dIdxVert>(coldPolys->getBuff()[ i+(2*sizeof(dIdxVert)) ])
				)
			);
		}

		mLazyMesh.get().mFresh = true;
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

	tArray<dGLFloat> * buffV = new tArray<dGLFloat>(vertCount * DIMENSIONS);
	tAutoPtr<iArray> ptrBuffV(buffV);

	tArray<dIdxV> * buffI = new tArray<dIdxV>(polyCount * VERTS_PER_POLY);
	tAutoPtr<iArray> ptrBuffI(buffI);

	try{
		for(size_t iVert=0; iVert < vertCount; ++iVert){
			buffV->mData[iVert + IOA_X] = static_cast<dGLFloat>( ref.mVertexes[iVert].x );
			buffV->mData[iVert + IOA_Y] = static_cast<dGLFloat>( ref.mVertexes[iVert].y );
			buffV->mData[iVert + IOA_Z] = static_cast<dGLFloat>( ref.mVertexes[iVert].z );
		}

		glGenBuffers(1, &mVBO);		// Generate 1 vertex array.
		glBindBuffer(GL_ARRAY_BUFFER, mVBO);
		glEnableVertexAttribArray(0);	//- positions in stream 0.
		glVertexPointer(DIMENSIONS, GL_FLOAT, 0, NULL);
		glBufferData(
			GL_ARRAY_BUFFER,
			buffV->getSize(),
			buffV->mData,
			GL_STATIC_DRAW
		);

		size_t iPoly=0;
		for(
			std::vector<sPoly>::iterator itrP = ref.mPolys.begin();
			itrP != ref.mPolys.end();
			++itrP
		){
			ASRT_INRANGE(ref.mVertexes, itrP->a);
			buffI->mData[iPoly++] = static_cast<dIdxV>(itrP->a);

			ASRT_INRANGE(ref.mVertexes, itrP->b);
			buffI->mData[iPoly++] = static_cast<dIdxV>(itrP->b);

			ASRT_INRANGE(ref.mVertexes, itrP->c);
			buffI->mData[iPoly++] = static_cast<dIdxV>(itrP->c);
		}

		glGenBuffers(1, &mIBO);		// Generate 1 index buffer
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mIBO);
		glBufferData(
			GL_ELEMENT_ARRAY_BUFFER,
			buffI->getSize(),
			buffI->mData,
			GL_STATIC_DRAW
		);

		cFridge * f = gWorld.get()->getFridge();
		mCache = makeFridgeID(buffV->mData, vertCount * DIMENSIONS, buffI->mData, polyCount * DIMENSIONS);
		f->chill(mCache, ptrBuffV);
		mCache = mCache << 1;
		f->chill(mCache, ptrBuffI);
		mCache = mCache >> 1;

	}catch(excep::base_error &e){
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
		glDeleteBuffers(1, &mIBO);
		mIBO = 0;

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glDeleteBuffers(1, &mVBO);
		mVBO = 0;

		WARN(e);
	}

	cleanLazy();
}

