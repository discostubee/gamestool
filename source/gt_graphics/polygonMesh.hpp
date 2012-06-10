/*
 * !\file	polygonMesh.hpp
 * !\brief	Contains the polygon figment as well as all its data types.
 */

#ifndef POLYGONMESH_HPP
#define POLYGONMESH_HPP

#include "windowFrame.hpp"

namespace gt{
	typedef unsigned int dIdxVert;

	struct sVertex{
		dUnitVDis x, y, z;
		sVertex() : x(0.0), y(0.0), z(0.0) {}
		sVertex(dUnitVDis pX, dUnitVDis pY, dUnitVDis pZ) : x(pX), y(pY), z(pZ) {}
		~sVertex(){}
	};

	struct sLine{
		dIdxVert a, b;
		sLine() : a(0), b(0) {}
		sLine( dIdxVert pA, dIdxVert pB ) : a(pA), b(pB) {}
		~sLine(){}
	};

	struct sPoly{
		dIdxVert a, b, c;
		s3DVec surfNorm;
		s3DVec normA, normB, normC;

		sPoly() : a(0), b(0), c(0) {}
		sPoly(dIdxVert aA, dIdxVert aB, dIdxVert aC) : a(aA), b(aB), c(aC) {}
	};

	//!\brief	Used to pass a mesh in an abstract manner.
	struct sMesh{
		std::vector<sVertex> mVertexes;
		std::vector<sPoly> mPolys;
	};

	//!\brief	This class is meant to be a frontend for a native version, that takes a generic mesh format and creates a more efficient native version.
	//!			The generic version is then deleted to save memory.
	//!			When run, this figment should render a polygon mesh.
	class cPolyMesh: public cFigment, private tOutline<cPolyMesh>{
	public:
		//-----------------------------
		const static cPlugTag* xPT_Mesh;	// Expects the sMesh struct.
		const static cCommand::dUID xAddVert;	//!< Adds vertexes as a pile.
		const static cCommand::dUID xAddPoly;	//!< Adds polygons as a pile.
		const static cCommand::dUID xGetMesh;	//!< Expects the xPT_Mesh, which it changes to be the generic version of the current mesh.

		//-----------------------------
		// The stuff me must have.
		static const char* identify(){ return "polygon mesh"; }

		virtual dNameHash hash() const { return tOutline<cPolyMesh>::hash(); }
		virtual const char* name() const{ return identify(); }		//!< Virtual version of identify.

		//-----------------------------
		cPolyMesh();
		virtual ~cPolyMesh();

	protected:
		sMesh* mLazyMesh;	//!<	the idea here is that we fill the lazy mesh up with data, and then on its next run, we take that data and fill up
							//!		native format mesh. Then we delete the lazy mesh.

		void patAddVert(ptrLead aLead);
		void patAddPoly(ptrLead aLead);
		void patGetMesh(ptrLead aLead);

		void promiseLazy();	//!< Make sure the lazy mesh is there.
		void cleanLazy();	//!< Just a little something to remind you you have to manage the lazy mesh.

		virtual sMesh getCurrentMesh();	//!< Returns a generic mesh.
	};
}

///////////////////////////////////////////////////////////////////////////////////
// template specializations
namespace gt{

	template<>
	class tPlugFlakes<sMesh>: public cBase_plug{
	public:
		tPlugFlakes(dPlugType pTI, dMapCopiers *pCopiers) :
			cBase_plug(pTI, pCopiers)
		{}

		virtual ~tPlugFlakes(){}

		virtual void save(cByteBuffer* pSaveHere){
			sMesh &m = get();
			size_t tmpSize, outSize;
			dByte *tmpBuff=NULL;

			tmpSize = m.mPolys.size();
			bpk::pack(&tmpSize, &tmpBuff, &outSize);
			pSaveHere->add(tmpBuff, outSize);
			delete tmpBuff;
		}

		virtual void loadEat(cByteBuffer* pChewToy, dReloadMap *aReloads = NULL){

		}

		virtual sMesh& get() = 0;
	};

}

#endif
