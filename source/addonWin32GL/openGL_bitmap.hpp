#ifndef OPENGL_BITMAP_HPP
#define OPENGL_BITMAP_HPP

#include "openGL_layer.hpp"
#include <gt_graphics/bitmap.hpp>

namespace gt{

	class cBitmap_gl: public cBitmap, public tAddonDependant<cWinGLAddon>{
	public:

		cBitmap_gl();
		virtual ~cBitmap_gl();

		GT_IDENTIFY("bitmap gl");
		GT_REPLACES(cBitmap);
	};
}

#endif
