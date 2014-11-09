/*
 * !\file	camera.hpp
 * !\brief
 */

#ifndef OPENGL_CAMERA_HPP
#define OPENGL_CAMERA_HPP

#include "openGL_stage.hpp"
#include "gt_graphics/camera.hpp"

namespace gt{

	class c3DCamera_X11GL: public cCamera3D{
	public:
		GT_IDENTIFY("camera 3d gl");
		GT_REPLACES(cCamera3D);
		virtual dNameHash hash() const { return getHash<c3DCamera_X11GL>(); }

		c3DCamera_X11GL();
		virtual ~c3DCamera_X11GL();
	};

	class c2DCamera_X11GL: public cCamera2D{
	public:
		GT_IDENTIFY("camera 2d gl");
		GT_REPLACES(cCamera2D);
		virtual dNameHash hash() const { return getHash<c2DCamera_X11GL>(); }

		c2DCamera_X11GL();
		virtual ~c2DCamera_X11GL();
	};
}

#endif
