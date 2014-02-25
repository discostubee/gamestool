/*
 * !\file	polygonMesh.hpp
 * !\brief	Contains the polygon figment as well as all its data types.
 */

#ifndef POLYGONMESH_HPP
#define POLYGONMESH_HPP

#include "stage.hpp"
#include "gt_base/plugContainer.hpp"

///////////////////////////////////////////////////////////////////////////////////
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

	struct sLine{
		dIdxVert a, b;

		sLine() : a(0), b(0) {}
		sLine( dIdxVert pA, dIdxVert pB ) : a(pA), b(pB) {}
		~sLine(){}

		sLine& operator+= (const sLine &aCopyMe);
	};

	struct sTexMap{
		dUnitVDis u[4], v[4];
		unsigned int idxBMap;	//!< Index into the bitmap list.

		sTexMap() : idxBMap(-1) { memset(u, 0, sizeof(u)); memset(v, 0, sizeof(u)); }

		sTexMap& operator+= (const sTexMap &aCopyMe);
	};

	struct sPoly{
		dIdxVert a, b, c;
		t3DVec<dUnitVDis> surfNorm;
		t3DVec<dUnitVDis> normA, normB, normC;

		sPoly() : a(0), b(0), c(0) {}
		sPoly(dIdxVert aA, dIdxVert aB, dIdxVert aC) : a(aA), b(aB), c(aC) {}

		sPoly& operator+= (const sPoly &aCopyMe);
	};

	//!\brief	Used to pass a mesh in an abstract manner.
	struct sMesh{
		std::vector<sVertex> mVertexes;
		std::vector<sPoly> mPolys;
		std::vector<sTexMap> mTMap;

		sMesh& operator+= (const sMesh &aCopyMe);
	};
}

///////////////////////////////////////////////////////////////////////////////////
// template specializations
namespace gt{

	//!\brief	Saving any class that is a dynamic size, we need a special plug flake type.
	template<>
	class tPlugFlakes<sMesh>: public tDataPlug<sMesh>{
	public:
		tPlugFlakes(){}
		tPlugFlakes(dPlugType pTI){}
		virtual ~tPlugFlakes(){}

		//!\todo
		virtual void save(cByteBuffer* pSaveHere){
			static const size_t LIMIT = sizeof(dUnitVDis);
			size_t tmpSize, outSize;
			dByte tmpBuff[LIMIT];

			//-
			tmpSize = get().mVertexes.size();
			bpk::pack(&tmpSize, tmpBuff, &outSize, LIMIT); pSaveHere->add(tmpBuff, outSize);

			for(std::vector<sVertex>::iterator itr = get().mVertexes.begin(); itr != get().mVertexes.end(); ++itr){
				bpk::pack(&itr->x, tmpBuff, &outSize, LIMIT); pSaveHere->add(tmpBuff, outSize);
				bpk::pack(&itr->y, tmpBuff, &outSize, LIMIT); pSaveHere->add(tmpBuff, outSize);
				bpk::pack(&itr->z, tmpBuff, &outSize, LIMIT); pSaveHere->add(tmpBuff, outSize);
			}

			//-
			tmpSize = get().mPolys.size();
			bpk::pack(&tmpSize, tmpBuff, &outSize, LIMIT); pSaveHere->add(tmpBuff, outSize);

			for(std::vector<sPoly>::iterator itr = get().mPolys.begin(); itr != get().mPolys.end(); ++itr){
				bpk::pack(&itr->a, tmpBuff, &outSize, LIMIT); pSaveHere->add(tmpBuff, outSize);
				bpk::pack(&itr->b, tmpBuff, &outSize, LIMIT); pSaveHere->add(tmpBuff, outSize);
				bpk::pack(&itr->c, tmpBuff, &outSize, LIMIT); pSaveHere->add(tmpBuff, outSize);
			}

			//-
			tmpSize = get().mTMap.size();
			bpk::pack(&tmpSize, tmpBuff, &outSize, LIMIT); pSaveHere->add(tmpBuff, outSize);

			for(std::vector<sTexMap>::iterator itr = get().mTMap.begin(); itr != get().mTMap.end(); ++itr){
				bpk::pack(itr->u, tmpBuff, &outSize, LIMIT); pSaveHere->add(tmpBuff, outSize);
				bpk::pack(itr->v, tmpBuff, &outSize, LIMIT); pSaveHere->add(tmpBuff, outSize);
				bpk::pack(&itr->idxBMap, tmpBuff, &outSize, LIMIT); pSaveHere->add(tmpBuff, outSize);
			}
		}

		//!\todo
		virtual void loadEat(cByteBuffer* pChewToy, dReloadMap *aReloads = NULL){
			size_t readPt=0;

			{
				size_t num=0;
				readPt += pChewToy->fill(&num, readPt);
				get().mVertexes.resize(readPt);
			}

			for(std::vector<sVertex>::iterator itr = get().mVertexes.begin(); itr != get().mVertexes.end(); ++itr){
				readPt += pChewToy->fill(&itr->x, readPt);
				readPt += pChewToy->fill(&itr->y, readPt);
				readPt += pChewToy->fill(&itr->z, readPt);
			}

			{
				size_t num=0;
				readPt += pChewToy->fill(&num, readPt);
				get().mPolys.reserve(readPt);
			}

			for(std::vector<sPoly>::iterator itr = get().mPolys.begin(); itr != get().mPolys.end(); ++itr){
				readPt += pChewToy->fill(&itr->a, readPt);
				readPt += pChewToy->fill(&itr->b, readPt);
				readPt += pChewToy->fill(&itr->c, readPt);
			}

			pChewToy->trimHead(readPt);
		}
	};
}

///////////////////////////////////////////////////////////////////////////////////
// template specializations
namespace gt{

	//----------------------------------------------------------------------------------------------------------------
	//!\brief	This class is meant to be a frontend for a native version, that takes a generic mesh format and creates
	//!			a more efficient native version. The generic version is then deleted to save memory. When run, this
	//!			figment should render a polygon mesh.
	//!\note	It's up to the implementation to detect new data in the lazy mesh and regenerate the native data.
	class cPolyMesh: public cFigment{
	public:
		static const cPlugTag* xPT_vertexs;
		static const cPlugTag* xPT_polies;
		static const cPlugTag* xPT_bitmaps;
		static const cPlugTag* xPT_texMapping;
		static const cPlugTag* xPT_box;
		static const cPlugTag* xPT_sphere;

		static const cCommand::dUID xAddToMesh;	//!< Adds any polies or vertexes to the current mesh. It does NOT clear the old mesh.
		static const cCommand::dUID xGetMesh;	//!< Expects the xPT_Mesh, which it changes to be the generic version of the current mesh.
		static const cCommand::dUID	xMeasure;	//!< Attempts to set a bounding box first, as a measurement of the mesh, and a sphere second.
		static const cCommand::dUID	xTexturize;	//!< Apply texture mapping.

		GT_IDENTIFY("poly mesh");
		GT_EXTENDS(cFigment);
		GT_VERSION(1);
		virtual dNameHash hash() const { return getHash<cPolyMesh>(); }

		cPolyMesh();
		virtual ~cPolyMesh();

	protected:
		typedef tPlugLinearContainer<ptrFig, std::list> dFigPlugList;

		tPlug<sMesh> mLazyMesh;	//!< The lazy mesh isn't always a complete mesh. It's a way for the implemented version to upload and download bits and pieces of information.
		dFigPlugList mBMaps;	//!< These are the bitmaps referenced by the lazy mesh. They are not the bitmaps used by the implementation, which is why they're not part of the lazy mesh.
		bool mUpdateLazy;

		void patAddToMesh(ptrLead aLead);
		void patGetMesh(ptrLead aLead);
		void patMeasure(ptrLead aLead);
		void patTexturize(ptrLead aLead);

		void cleanLazy();	//!< Just a little something to remind you you have to manage the lazy mesh.

		virtual void downloadLazy(){ DONT_USE_THIS; }	//!< The implementation needs to take its specific data and build the generic version.
		virtual void measure(geometry::tCube<dUnitVDis> &pOutCube){ DONT_USE_THIS; }
		virtual void measure(geometry::tSphere<dUnitVDis> &pOutSphere){ DONT_USE_THIS; }
	};
}


#endif
