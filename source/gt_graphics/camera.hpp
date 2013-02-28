/*
 * !\file	camera.hpp
 * !\brief
 */

#ifndef CAMERA_HPP
#define CAMERA_HPP

#include "stage.hpp"


namespace gt{

	class c3DCamera: public cFigment{
	public:

		c3DCamera();
		virtual ~c3DCamera();

		GT_IDENTIFY("3d camera");
		virtual dNameHash hash() const { return getHash<c3DCamera>(); }

	};

	class c2DCamera: public cFigment{
	public:

		c2DCamera();
		virtual ~c2DCamera();

		GT_IDENTIFY("2d camera");
		virtual dNameHash hash() const { return getHash<c2DCamera>(); }

	};
}

#endif
