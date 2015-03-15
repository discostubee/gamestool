/*
 * !\file	polygonMesh.hpp
 * !\brief	Contains the polygon figment as well as all its data types.
 */

#ifndef POLYGONMESH_HPP
#define POLYGONMESH_HPP

#include "stage.hpp"
#include "gt_base/plugContainerOps.hpp"

///////////////////////////////////////////////////////////////////////////////////
namespace gt{
	typedef unsigned int dIdxVert;

	//----------------------------------------------------------------------------------------------------------------
	struct sVertex{
		enum eVert { eX, eY, eZ, eNone };

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

		sTexMap() { memset(u, 0, sizeof(u)); memset(v, 0, sizeof(u)); }

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
		bool mFresh;
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
			size_t num;

			//-
			num = get().mVertexes.size();
			pSaveHere->add(&num);

			for(std::vector<sVertex>::iterator itr = get().mVertexes.begin(); itr != get().mVertexes.end(); ++itr){
				pSaveHere->add(&itr->x);
				pSaveHere->add(&itr->y);
				pSaveHere->add(&itr->z);
			}

			//-
			num = get().mPolys.size();
			pSaveHere->add(&num);

			for(std::vector<sPoly>::iterator itr = get().mPolys.begin(); itr != get().mPolys.end(); ++itr){
				pSaveHere->add(&itr->a);
				pSaveHere->add(&itr->b);
				pSaveHere->add(&itr->c);
			}

			//-
			num = get().mTMap.size();
			pSaveHere->add(&num);

			for(std::vector<sTexMap>::iterator itr = get().mTMap.begin(); itr != get().mTMap.end(); ++itr){
				pSaveHere->add(reinterpret_cast<dByte*>(itr->u), 4 * sizeof(dUnitVDis));
				pSaveHere->add(reinterpret_cast<dByte*>(itr->v), 4 * sizeof(dUnitVDis));
			}
		}

		//!\todo
		virtual void loadEat(cByteBuffer* pChewToy, dReloadMap *aReloads = NULL){
			size_t readPt=0;
			size_t num=0;

			readPt += pChewToy->fill(&num, readPt);
			get().mVertexes.resize(num);

			for(std::vector<sVertex>::iterator itr = get().mVertexes.begin(); itr != get().mVertexes.end(); ++itr){
				readPt += pChewToy->fill(&itr->x, readPt);
				readPt += pChewToy->fill(&itr->y, readPt);
				readPt += pChewToy->fill(&itr->z, readPt);
			}

			readPt += pChewToy->fill(&num, readPt);
			get().mPolys.resize(num);

			for(std::vector<sPoly>::iterator itr = get().mPolys.begin(); itr != get().mPolys.end(); ++itr){
				readPt += pChewToy->fill(&itr->a, readPt);
				readPt += pChewToy->fill(&itr->b, readPt);
				readPt += pChewToy->fill(&itr->c, readPt);
			}

			readPt += pChewToy->fill(&num, readPt);
			get().mTMap.resize(num);

			for(std::vector<sTexMap>::iterator itr = get().mTMap.begin(); itr != get().mTMap.end(); ++itr){
				readPt += pChewToy->arrFill(itr->u, 4, readPt);
				readPt += pChewToy->arrFill(itr->v, 4, readPt);
			}

			pChewToy->trimHead(readPt);
			get().mFresh = true;
		}
	};

	template<>
	class cAnyOp::tOps< tPlugLinearContainer<sVertex, std::vector>::dContainer >{
	public:
		typedef tPlugLinearContainer<sVertex, std::vector>::dContainer dVertPlugs;

	private:

		static void assignText(const dVertPlugs & pFrom, void * pTo){
			reinterpret_cast<dText*>(pTo)->t.clear();
			appendText(pFrom, pTo);
		}

		static void appendText(const dVertPlugs & pFrom, void * pTo){
			std::stringstream ss;
			for(dVertPlugs::const_iterator vert = pFrom.begin(); vert != pFrom.end(); ++vert){
				ss << vert->getConst().x
					<< ", " << vert->getConst().y
					<< ", " << vert->getConst().z
					<< ";" << std::endl;
			}

			reinterpret_cast<dText*>(pTo)->t += ss.str();
		}

		static void appendFromText(const dText & pFrom, void * pTo){
			std::string token;
			std::istringstream streamAll(pFrom.t);
			dVertPlugs* verts = reinterpret_cast< dVertPlugs* >(pTo);
			sVertex tmpV;
			sVertex::eVert e;

			while(std::getline(streamAll, token, ';')){
				e = sVertex::eX;
				std::istringstream streamV(token);
				while(e != sVertex::eNone){
					if(std::getline(
						streamV,
						token,
						','
					)){
						switch(e){
						case sVertex::eX:
							tmpV.x = ::atof(token.c_str());
							e = sVertex::eY;
							break;
						case sVertex::eY:
							tmpV.y = ::atof(token.c_str());
							e = sVertex::eZ;
							break;
						case sVertex::eZ:
							tmpV.z = ::atof(token.c_str());
							e = sVertex::eNone;
							break;
						default:
							break;
						}
					}else{
						break;
					}
				}
				verts->push_back(tmpV);
			}
		}

		static void assignFromText(const dText & pFrom, void * pTo){
			dVertPlugs* verts = reinterpret_cast< dVertPlugs* >(pTo);
			verts->clear();
			appendFromText(pFrom, pTo);
		}

	public:

		static void setup(tKat< dVertPlugs > * pK, cAnyOp * pUsing){
			std::cout << "hello" << std::endl; //!!!
			pK->addAss(&getRef(), genPlugType<dText>(), assignText);
			pK->addApp(&getRef(), genPlugType<dText>(), appendText);
			cAnyOp::tKat<dText>::xKat.addApp(
				&getRef(),
				genPlugType<dVertPlugs>(),
				appendFromText
			);
			cAnyOp::tKat<dText>::xKat.addAss(
				&getRef(),
				genPlugType<dVertPlugs>(),
				assignFromText
			);
		}
	};


}

///////////////////////////////////////////////////////////////////////////////////
// Class
namespace gt{

	//----------------------------------------------------------------------------------------------------------------
	//!\brief	This class is meant to be a frontend for a native version, that takes a generic mesh format and creates
	//!			a more efficient native version. The generic version is then deleted to save memory. When run, this
	//!			figment should render a polygon mesh.
	//!\note	It's up to the implementation to detect new data in the lazy mesh and regenerate the native data.
	class cPolyMesh: public cFigment{
	public:
		typedef tPlugLinearContainer< sVertex, std::vector > dPlugVerts;
		typedef tPlugLinearContainer< sPoly, std::vector > dPlugPoly;

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

		GT_IDENTIFY("polymesh");
		GT_EXTENDS(cFigment);
		GT_VERSION(1);
		virtual dNameHash hash() const { return getHash<cPolyMesh>(); }

		cPolyMesh();
		virtual ~cPolyMesh();

	protected:
		typedef tPlugLinearContainer<ptrFig, std::list> dFigPlugList;

		tPlug<sMesh> mLazyMesh;	//!< The lazy mesh isn't always a complete mesh. It's a way for the implemented version to upload and download bits and pieces of information.
		dFigPlugList mBMaps;	//!< These are the bitmaps referenced by the lazy mesh. They are not the bitmaps used by the implementation, which is why they're not part of the lazy mesh.

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
