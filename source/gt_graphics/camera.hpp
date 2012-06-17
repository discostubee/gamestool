/*
 * !\file	camera.hpp
 * !\brief
 */

#ifndef CAMERA_HPP
#define CAMERA_HPP

#include "windowFrame.hpp"


namespace gt{

	class c3DCamera: public cFigment, private tOutline<c3DCamera>{
	public:

		//--- Required
		static const dPlaChar* identify(){ return "3D camera"; }
		virtual const dPlaChar* name() const { return identify(); }		//!< Virtual version of identify.
		virtual dNameHash hash() const { return tOutline<c3DCamera>::hash(); }

		//--- Standard
		c3DCamera();
		virtual ~c3DCamera();

	};

	class c2DCamera: public cFigment, private tOutline<c2DCamera>{
	public:

		//--- Required
		static const dPlaChar* identify(){ return "2D camera"; }
		virtual const dPlaChar* name() const { return identify(); }		//!< Virtual version of identify.
		virtual dNameHash hash() const { return tOutline<c2DCamera>::hash(); }

		//--- Standard
		c2DCamera();
		virtual ~c2DCamera();

	};
}

#endif
