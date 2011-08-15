/*
 *
 * !\file
 * !\brief
 */

#ifndef OPENGL_LAYER_HPP
#define OPENGL_LAYER_HPP

#include <gt_graphics/layer.hpp>
#include "openGL_windowFrame.hpp"


namespace gt{

	class cLayer_X11GL: public cLayer, private tOutline<cLayer_X11GL>{
	protected:
		tPlug<GLfloat>	mX, mY;
		tPlug<GLfloat>	mWidth, mHeight;

		virtual void setDim(dUnitPix pX, dUnitPix pY, dUnitPix pW, dUnitPix pH);	//!< Sets dimensions of frame.

	public:
		static const char* identify(){ return "layer X11GL"; }
		static dNameHash replaces(){ return getHash<cLayer>(); }

		cLayer_X11GL();
		virtual ~cLayer_X11GL();

		virtual const char* name() const{ return identify(); }
		virtual dNameHash hash() const{ return tOutline<cLayer_X11GL>::hash(); }
		virtual void run(cContext* pCon);
	};
}

#endif
