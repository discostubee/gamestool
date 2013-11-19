#ifndef OPENGL_BITMAP_HPP
#define OPENGL_BITMAP_HPP

#include "openGL_stage.hpp"
#include "gt_graphics/bitmap.hpp"

namespace gt{

	class cBitmap_X11GL: public cBitmap, public tAddonDependent<X11GLAddon>{

	public:
		//-----------------------------


		//-----------------------------
		cBitmap_X11GL();
		virtual ~cBitmap_X11GL();

		GT_IDENTIFY("bitmap gl");
		GT_REPLACES(cBitmap);
		virtual dNameHash hash() const { return getHash<cBitmap_X11GL>(); }

		virtual void run(cContext *pCon);	//!< Applies all the dirty rects (hehe).

	protected:
		virtual ptrBitmap getBitmap();

	};
}

#endif
