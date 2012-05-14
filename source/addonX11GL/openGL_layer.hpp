/*
 *
 * !\file
 * !\brief
 */

#ifndef OPENGL_LAYER_HPP
#define OPENGL_LAYER_HPP

#include "openGL_windowFrame.hpp"

#include "gt_graphics/layer.hpp"

namespace gt{

	class cLayer_X11GL: public cLayer, private tOutline<cLayer_X11GL>{
	public:

		//--- Required
		static const char* identify(){ return "layer X11GL"; }
		virtual dNameHash hash() const{ return tOutline<cLayer_X11GL>::hash(); }
		virtual const char* name() const{ return identify(); }

		//--- Regular.
		cLayer_X11GL();
		virtual ~cLayer_X11GL();

		static dNameHash replaces(){ return getHash<cLayer>(); }
		virtual void run(cContext* pCon);
	};
}

#endif
