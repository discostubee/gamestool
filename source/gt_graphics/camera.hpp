/*
 * !\file	camera.hpp
 * !\brief
 */

#ifndef CAMERA_HPP
#define CAMERA_HPP

#include "stage.hpp"

namespace gt{

	class cCamera3D: public cChainLink{
	public:
		cCamera3D();
		virtual ~cCamera3D();

		GT_IDENTIFY("camera 3d");
		GT_EXTENDS(cChainLink);
		virtual dNameHash hash() const { return getHash<cCamera3D>(); }
	};

	class cCamera2D: public cChainLink{
	public:
		cCamera2D();
		virtual ~cCamera2D();

		GT_IDENTIFY("camera 2d");
		GT_EXTENDS(cChainLink);
		virtual dNameHash hash() const { return getHash<cCamera2D>(); }
	};
}

#endif
