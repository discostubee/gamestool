/*
 * !\file	camera.hpp
 * !\brief
 */

#ifndef CAMERA_HPP
#define CAMERA_HPP

#include "stage.hpp"

namespace gt{

	class c3DCamera: public cChainLink{
	public:
		c3DCamera();
		virtual ~c3DCamera();

		GT_IDENTIFY("camera 3d");
		GT_EXTENDS(cChainLink);
		virtual dNameHash hash() const { return getHash<c3DCamera>(); }
	};

	class c2DCamera: public cChainLink{
	public:
		c2DCamera();
		virtual ~c2DCamera();

		GT_IDENTIFY("camera 2d");
		GT_EXTENDS(cChainLink);
		virtual dNameHash hash() const { return getHash<c2DCamera>(); }
	};
}

#endif
