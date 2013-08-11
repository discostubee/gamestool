/*
 * !\file	polygonMesh.hpp
 * !\brief	Contains the polygon figment as well as all its data types.
 */

#ifndef POLYGONMESH_HPP
#define POLYGONMESH_HPP

#include "stage.hpp"

namespace gt{
	typedef unsigned int dIdxVert;

	//----------------------------------------------------------------------------------------------------------------
	struct sVertex{
		dUnitVDis x, y, z;

		sVertex() : x(0.0), y(0.0), z(0.0) {}
		sVertex(dUnitVDis pX, dUnitVDis pY, dUnitVDis pZ) : x(pX), y(pY), z(pZ) {}
		~sVertex(){}

		sVertex& operator+= (const sVertex &aCopyMe);
	};

	//----------------------------------------------------------------------------------------------------------------
	struct sLine{
		dIdxVert a, b;

		sLine() : a(0), b(0) {}
		sLine( dIdxVert pA, dIdxVert pB ) : a(pA), b(pB) {}
		~sLine(){}

		sLine& operator+= (const sLine &aCopyMe);
	};

	//----------------------------------------------------------------------------------------------------------------
	struct sPoly{
		dIdxVert a, b, c;
		t3DVec<dUnitVDis> surfNorm;
		t3DVec<dUnitVDis> normA, normB, normC;

		sPoly() : a(0), b(0), c(0) {}
		sPoly(dIdxVert aA, dIdxVert aB, dIdxVert aC) : a(aA), b(aB), c(aC) {}

		sPoly& operator+= (const sPoly &aCopyMe);
	};

	//----------------------------------------------------------------------------------------------------------------
	//!\brief	Used to pass a mesh in an abstract manner.
	struct sMesh{
		std::vector<sVertex> mVertexes;
		std::vector<sPoly> mPolys;

		sMesh& operator+= (const sMesh &aCopyMe);
	};

	//----------------------------------------------------------------------------------------------------------------
	//!\brief	This class is meant to be a frontend for a native version, that takes a generic mesh format and creates
	//!			a more efficient native version. The generic version is then deleted to save memory. When run, this
	//!			figment should render a polygon mesh.
	class cPolyMesh: public cFigment{
	public:
		const static cPlugTag* xPT_vert;
		const static cPlugTag* xPT_poly;

		const static cCommand::dUID xAddToMesh;	//!<
		const static cCommand::dUID xGetMesh;	//!< Expects the xPT_Mesh, which it changes to be the generic version of the current mesh.

		GT_IDENTIFY("polymesh");
		virtual dNameHash hash() const { return getHash<cPolyMesh>(); }

		cPolyMesh();
		virtual ~cPolyMesh();

	protected:
		sMesh* mLazyMesh;	//!<	the idea here is that we fill the lazy mesh up with data, and then on its next run, we take that data and fill up native format mesh. Then we delete the lazy mesh.

		void patAddToMesh(ptrLead aLead);
		void patGetMesh(ptrLead aLead);

		void promiseLazy();	//!< Make sure the lazy mesh is there.
		void cleanLazy();	//!< Just a little something to remind you you have to manage the lazy mesh.

		virtual sMesh getCurrentMesh();	//!< Returns a generic mesh.
	};
}

///////////////////////////////////////////////////////////////////////////////////
// template specializations
namespace gt{

	//!\brief	Saving any class that is a dynamic size, we need a special plug flake type.
	template<>
	class tPlugFlakes<sMesh>: public tDataPlug<sMesh>{
	public:
		tPlugFlakes(dPlugType pTI){}
		virtual ~tPlugFlakes(){}

		//!\todo
		virtual void save(cByteBuffer* pSaveHere){
			sMesh &m = get();
			size_t tmpSize, outSize;
			dByte *tmpBuff=NULL;

			tmpSize = m.mPolys.size();
			bpk::pack(&tmpSize, &tmpBuff, &outSize);
			pSaveHere->add(tmpBuff, outSize);
			SAFEDEL_ARR(tmpBuff);

			for(std::vector<sPoly>::iterator itr = m.mPolys.begin(); itr != m.mPolys.end(); ++itr){
				bpk::pack(&itr->a, &tmpBuff, &outSize);		SAFEDEL_ARR(tmpBuff);
				bpk::pack(&itr->b, &tmpBuff, &outSize);		SAFEDEL_ARR(tmpBuff);
				bpk::pack(&itr->c, &tmpBuff, &outSize);		SAFEDEL_ARR(tmpBuff);
			}
		}

		//!\todo
		virtual void loadEat(cByteBuffer* pChewToy, dReloadMap *aReloads = NULL){

		}
	};

}

#endif
