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

	class cLayer_winGL: public cLayer, private tOutline<cLayer_winGL>{
	public:
		static const dNatChar* identify(){ return "gl layer"; }
		static dNameHash replaces(){ return getHash<cLayer>(); }

		cLayer_winGL();
		virtual ~cLayer_winGL();

		virtual const dNatChar* name() const { return cLayer_winGL::identify(); }		//!<
		virtual dNameHash hash() const { return tOutline<cLayer_winGL>::hash(); }

		virtual void run(cContext* pCon);
	};
}

#endif
