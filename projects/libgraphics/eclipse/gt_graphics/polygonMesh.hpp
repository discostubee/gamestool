/*
 * !\file	polygonMesh.hpp
 * !\brief	Contains the polygon figment as well as all its data types.
 */

#ifndef POLYGONMESH_HPP
#define POLYGONMESH_HPP

#include <gt_base/figment.hpp>

namespace gt{


	struct sVertex{
		dUnitVD x, y, z;
		sVertex() : x(0.0), y(0.0), z(0.0) {}
		sVertex(dUnitVD pX, dUnitVD pY, dUnitVD pZ) : x(pX), y(pY), z(pZ) {}
		~sVertex(){}
	};

	struct sLine{
		sVertex *a, *b;
		sLine() : a(NULL), b(NULL) {}
		sLine( sVertex* pA, sVertex* pB ) : a(pA), b(pB) {}
		~sLine(){}

	};

	struct sPoly{
		sVertex *a, *b, *c;
		::s3DVec surfNorm;
		::s3DVec normA, normB, normC;

		sPoly() : a(NULL), b(NULL), c(NULL) {}
	};

	class cPolyMesh: public cFigment, private tOutline<cPolyMesh>{
	public:
		//-----------------------------
		// Defines

		//-----------------------------
		// Statics

		static const char* identify(){ return "polygon mesh"; }

		//-----------------------------
		cPolyMesh();
		virtual ~cPolyMesh();

		//-----------------------------
		// Constant, standard polymorphs
		virtual const char* name() const{ return identify(); }		//!< Virtual version of identify.

	};
}

#endif
