/*
 * !\file
 */
#ifndef OPENGL_TRANSFORM_HPP
#define OPENGL_TRANSFORM_HPP

#include "openGL_stage.hpp"
#include "gt_graphics/transform.hpp"

namespace gt{

class cTransform_X11GL: public cTransform{
public:
	cTransform_X11GL();
	virtual ~cTransform_X11GL();

	GT_IDENTIFY("transform gl");
	GT_REPLACES(cTransform);
	virtual dNameHash hash() const{ return getHash<cTransform_X11GL>(); }

	virtual void work(cContext* pCon);

protected:
	void getTransforms();
	void setTransforms();
};

}

#endif
