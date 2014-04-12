
/*
 * !\file	openGL_screens.hpp
 */
#ifndef OPENGL_SCREENS_HPP
#define OPENGL_SCREENS_HPP


#include "openGL_stage.hpp"
#include "gt_graphics/screens.hpp"

namespace gt{

class cScreen_X11GL: public cScreen{
public:
	cScreen_X11GL();
	virtual ~cScreen_X11GL();

	GT_IDENTIFY("screen gl");
	GT_REPLACES(cScreen);
	virtual dNameHash hash() const{ return getHash<cScreen_X11GL>(); }

protected:
	shape::tRectangle<dUnitPix> getDims();
};

}

#endif
