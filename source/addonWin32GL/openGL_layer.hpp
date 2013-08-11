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

	class cLayer_winGL: public cLayer, public tAddonDependant<cWinGLAddon>{
	public:
		cLayer_winGL();
		virtual ~cLayer_winGL();

		GT_IDENTIFY("layer gl");
		GT_REPLACES(cLayer);
		virtual dNameHash hash() const { return tOutline<cLayer_winGL>::hash(); }

		virtual void run(cContext* pCon);
	};
}

#endif
