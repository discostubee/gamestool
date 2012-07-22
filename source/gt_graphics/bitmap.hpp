/*
 * !\file	polygonMesh.hpp
 * !\brief	Contains the polygon figment as well as all its data types.
 */

#ifndef BITMAP_HPP
#define BITMAP_HPP

#include "gt_base/figment.hpp"

namespace gt{
	typedef float dColRng; //!< Colour range
	typedef unsigned short dColourChan; //!< Colour channel

	struct sRGB{
		dColRng r, g, b;
		sRGB() : r(0.0), g(0.0), b(0.0) {}
		sRGB(dColRng pR, dColRng pG, dColRng pB) : r(pR), g(pG), b(pB) {}
	};

	struct sRGBA : public sRGB{
		dColRng a;
		sRGBA() : a(0.0) {}
		sRGBA(dColRng pR, dColRng pG, dColRng pB, dColRng pA) : sRGB(pR, pG, pB), a(pA) {}
	};

}

namespace gt{

	class cBitmap : public cFigment, private tOutline<cBitmap>{

	protected:
		//-----------------------------
		// bitmap interface polymorphs
		virtual void setMap(dColourChan){ DONT_USE_THIS; };

	public:
		//-----------------------------
		// Defines

		//-----------------------------
		// Members


		//-----------------------------
		// Statics
		static const char* identify(){ return "bitmap"; }

		//-----------------------------
		cBitmap();
		virtual ~cBitmap();

		//-----------------------------
		// Standard, constant interface polymorphs
		virtual const char* name() const{ return identify(); }		//!< Virtual version of identify.

		//-----------------------------
		// Standard interface polymorphs
	};
}

#endif
