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

	class cLayer_X11GL: public cLayer, public tAddonDependent<X11GLAddon>{
	public:

		//--- Required
		GT_IDENTIFY("layer gl");
		GT_REPLACES(cLayer);
		virtual dNameHash hash() const { return getHash<cLayer_X11GL>(); }

		//--- Regular.
		cLayer_X11GL();
		virtual ~cLayer_X11GL();

		virtual void run(cContext* pCon);
	};
}

#endif
