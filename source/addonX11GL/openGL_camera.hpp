/*
 * !\file	camera.hpp
 * !\brief
 */

#ifndef OPENGL_CAMERA_HPP
#define OPENGL_CAMERA_HPP

#include "openGL_windowFrame.hpp"
#include "gt_graphics/camera.hpp"

namespace gt{

	class c3DCamera_X11GL: public c3DCamera, private tOutline<c3DCamera_X11GL>{
	public:

		//--- Required
		static const dPlaChar* identify(){ return "3D camera X11GL"; }
		virtual const dPlaChar* name() const { return identify(); }		//!< Virtual version of identify.
		virtual dNameHash hash() const { return tOutline<c3DCamera_X11GL>::hash(); }
		static dNameHash replaces(){ return getHash<c3DCamera>(); }

		c3DCamera_X11GL();
		virtual ~c3DCamera_X11GL();
	};

	class c2DCamera_X11GL: public c2DCamera, private tOutline<c2DCamera_X11GL>{
	public:

		//--- Required
		static const dPlaChar* identify(){ return "2D camera X11GL"; }
		virtual const dPlaChar* name() const { return identify(); }		//!< Virtual version of identify.
		virtual dNameHash hash() const { return tOutline<c2DCamera_X11GL>::hash(); }
		static dNameHash replaces(){ return getHash<c2DCamera>(); }

		c2DCamera_X11GL();
		virtual ~c2DCamera_X11GL();
	};
}

#endif
