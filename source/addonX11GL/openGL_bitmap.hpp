
/*
 * !\file	openGL_stage.hpp
 * !\note	This code comes from 'The game programmers wiki', so big props to them. http://gpwiki.org/index.php/OpenGL:Tutorials:Setting_up_OpenGL_on_X11
 * !\note	Will have to install: mesa-common-dev
 */
#ifndef OPENGL_BITMAP_HPP
#define OPENGL_BITMAP_HPP


#include "openGL_stage.hpp"
#include "gt_graphics/bitmap.hpp"

namespace gt{

class cBitmap_X11GL: public cBitmap{
public:
	cBitmap_X11GL();
	virtual ~cBitmap_X11GL();

	GT_IDENTIFY("bitmap gl");
	GT_REPLACES(cBitmap);
	virtual dNameHash hash() const{ return getHash<cBitmap_X11GL>(); }

protected:
	ptrBitmap getBitmap();
	void applyDirt(const dListDirty pDirt);
};

}

#endif
