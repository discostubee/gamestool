/*
 * !\file	camera.hpp
 * !\brief
 */

#ifndef CAMERA_HPP
#define CAMERA_HPP

#include "windowFrame.hpp"


namespace gt{

	class cCamera: public cFigment, private tOutline<cCamera>{
	public:
		static const cPlugTag *xPT_mode;
		static const cCommand *xSetDMode;	//!< 2D or 3D mode.

		enum{
			eSetDMode = cFigment::eSwitchEnd + 1,
			eSwitchEnd
		};

		//--- Required
		static const dNatChar* identify(){ return "camera"; }
		virtual const dNatChar* name() const { return identify(); }		//!< Virtual version of identify.
		virtual dNameHash hash() const { return tOutline<cCamera>::hash(); }

		//--- Standard
		cCamera();
		virtual ~cCamera();

		virtual void jack(ptrLead pLead, cContext *pCon);

	};
}

#endif
