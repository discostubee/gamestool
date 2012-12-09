/*
 * !\file	camera.hpp
 * !\brief
 */

#ifndef OPENGL_CAMERA_HPP
#define OPENGL_CAMERA_HPP

#include "openGL_windowFrame.hpp"
#include "gt_graphics/camera.hpp"

namespace gt{

	class c3DCamera_X11GL: public c3DCamera, public tAddonDependent<X11GLAddon>{
	public:

		//--- Required
		GT_IDENTIFY("3d camera gl");
		GT_REPLACES(c3DCamera);
		virtual dNameHash hash() const { return getHash<c3DCamera_X11GL>(); }

		c3DCamera_X11GL();
		virtual ~c3DCamera_X11GL();
	};

	class c2DCamera_X11GL: public c2DCamera, private tOutline<c2DCamera_X11GL>{
	public:

		//--- Required
		GT_IDENTIFY("2d camera gl");
		GT_REPLACES(c2DCamera);
		virtual dNameHash hash() const { return getHash<c2DCamera_X11GL>(); }

		c2DCamera_X11GL();
		virtual ~c2DCamera_X11GL();
	};
}

#endif
