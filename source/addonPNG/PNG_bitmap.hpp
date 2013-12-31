#ifndef OPENGL_BITMAP_HPP
#define OPENGL_BITMAP_HPP

#include "addonPNG.hpp"
#include "gt_graphics/bitmap.hpp"

namespace gt{

	//!\brief	Uses the PNG format to store bitmap images.
	class cBitmap_PNG: public cBitmap, public tAddonDependent<PNGAddon>{
	public:
		cBitmap_PNG();
		virtual ~cBitmap_PNG();

		GT_IDENTIFY("bitmap png");
		GT_REPLACES(cBitmap);
		virtual dNameHash hash() const { return getHash<cBitmap_PNG>(); }

		virtual void run(cContext *pCon);	//!< Applies all the dirty rects (hehe).

	protected:
		virtual ptrBitmap getBitmap();

	};
}

#endif
