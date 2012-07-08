#ifndef OPENGL_BITMAP_HPP
#define OPENGL_BITMAP_HPP

#include "openGL_layer.hpp"

#include "gt_graphics/bitmap.hpp"

namespace gt{

	class cBitmap_X11GL: public cBitmap, public tAddonDependent<X11GLAddon>{

	public:
		//-----------------------------


		//-----------------------------
		cBitmap_X11GL();
		virtual ~cBitmap_X11GL();

		static const char* identify(){ return "bitmap X11GL"; }
		virtual const char* name() const{ return identify(); }

		static dNameHash replaces(){ return getHash<cBitmap>(); }
		virtual dNameHash getReplacement() const{ return replaces(); }

		virtual dNameHash hash() const { return getHash<cBitmap_X11GL>(); }

		virtual void run(cContext *pCon);

	protected:
		virtual ptrBitmap getBitmap();

	};
}

#endif
