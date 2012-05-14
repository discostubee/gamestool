#ifndef OPENGL_BITMAP_HPP
#define OPENGL_BITMAP_HPP

#include "openGL_layer.hpp"
#include <gt_graphics/bitmap.hpp>

namespace gt{

	class cBitmap_gl: public cBitmap, private tOutline<cBitmap_gl>{

	protected:
		virtual void setMap(dColourChan){ DONT_USE_THIS; };

	public:
		//-----------------------------
		// Defines

		//-----------------------------
		// Members


		//-----------------------------
		// Statics

		static const char* identify(){ return "gl bitmap"; }
		static dNameHash replaces(){ return getHash<cBitmap>(); }

		//-----------------------------
		cBitmap_gl();
		virtual ~cBitmap_gl();

		//-----------------------------
		virtual const char* name() const{ return identify(); }		//!< Virtual version of identify.

	};
}

#endif
