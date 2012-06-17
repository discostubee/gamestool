#ifndef OPENGL_BITMAP_HPP
#define OPENGL_BITMAP_HPP

#include "openGL_layer.hpp"

#include "gt_graphics/bitmap.hpp"

namespace gt{

	class cBitmap_X11GL: public cBitmap, private tOutline<cBitmap_X11GL>{

	protected:
		virtual void setMap(dColourChan){ DONT_USE_THIS; };

	public:
		//-----------------------------
		// Defines

		//-----------------------------
		// Members


		//-----------------------------
		// Statics

		static const char* identify(){ return "bitmap X11GL"; }
		static dNameHash replaces(){ return getHash<cBitmap>(); }

		//-----------------------------
		cBitmap_X11GL();
		virtual ~cBitmap_X11GL();

		//-----------------------------
		virtual const char* name() const{ return identify(); }
		virtual dNameHash getReplacement() const{ return replaces(); }

	};
}

#endif
