/*
 * !\file	polygonMesh.hpp
 * !\brief	Contains the polygon figment as well as all its data types.
 */

#ifndef POLYGONMESH_HPP
#define POLYGONMESH_HPP

#include <gt_base/figment.hpp>

namespace gt{
	typedef unsigned int dIdxVert;

	struct sVertex{
		dUnitVD x, y, z;
		sVertex() : x(0.0), y(0.0), z(0.0) {}
		sVertex(dUnitVD pX, dUnitVD pY, dUnitVD pZ) : x(pX), y(pY), z(pZ) {}
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
		::s3DVec surfNorm;
		::s3DVec normA, normB, normC;

		sPoly() : a(0), b(0), c(0) {}
	};

	//!\brief	Used to pass a mesh in an abstract manner.
	struct sMesh{
		std::vector<sVertex> mVertexes;
		std::vector<sPoly> mPolys;
	};

	class cPolyMesh: public cFigment, private tOutline<cPolyMesh>{
	public:
		//-----------------------------
		const static cPlugTag* xPT_Vert;	// Expects the sVertex struct.
		const static cPlugTag* xPT_Poly;	// Expects the sPoly struct.
		const static cPlugTag* xPT_Mesh;	// Expects the sMesh struct.
		const static cCommand* xAddVert;	//
		const static cCommand* xAddPoly;	//
		const static cCommand* xGetMesh;	// Returns an sMesh instance.

		enum{
			eAddVert = cFigment::eSwitchEnd + 1,
			eAddPoly,
			eGetMesh,
			eSwitchEnd
		};

		//-----------------------------
		// The stuff me must have.
		static const char* identify(){ return "polygon mesh"; }

		virtual dNameHash hash() const { return tOutline<cPolyMesh>::hash(); }
		virtual const char* name() const{ return identify(); }		//!< Virtual version of identify.

		//-----------------------------
		cPolyMesh();
		virtual ~cPolyMesh();

		virtual void jack(ptrLead pLead);

	protected:
		sMesh* mLazyMesh;	//!<	the idea here is that we fill the lazy mesh up with data, and then on its next run, we take that data and fill up
							//!		native format mesh. Then we delete the lazy mesh.

		void promiseLazy();	//!< Make sure the lazy mesh is there.
		void cleanLazy();	//!< Just a little something to remind you you have to manage the lazy mesh.

		virtual sMesh getCurrentMesh();	//!< Should return the native mesh, ignoring any lazy mesh. Or, the lazy mesh if there is no native mesh.
	};
}

///////////////////////////////////////////////////////////////////////////////////
// template specializations
namespace gt{

	//!\brief	sMesh requires a special plug in order to work.
	template<>
	class cPlug<sMesh>: public cBase_plug{
	public:
		sMesh mD;

		cPlug() : cBase_plug(typeid(sMesh)){}
		cPlug(const sMesh& pMesh) : cBase_plug(typeid(sMesh)), mD(pMesh){}
		virtual ~cPlug(){}

		virtual cBase_plug& operator= (const cBase_plug &pD){ if(&pD != this){} return *this;}
		virtual void operator= (const sMesh& pA){}

		virtual cByteBuffer& save(){ cByteBuffer meshSave; return meshSave; }
		virtual void loadEat(cByteBuffer* pChewToy, dReloadMap* pReloads){}
		virtual void reset(){ mD = sMesh();}
	};
}

#endif
