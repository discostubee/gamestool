/*
 * !\file	window.hpp
 * !\brief
 */

#ifndef WINDOWFRAME_HPP
#define WINDOWFRAME_HPP

#include <gt_base/figment.hpp>
#include "primitives.hpp"

namespace gt{
	//!\class	cWindow
	//!\brief	The window class provides a typical window interface we see in most OSs.
	//!\note	Runs the linked figment when you close this window.
	class cWindowFrame: public cFigment, private tOutline<cWindowFrame>{
	public:
		static const cPlugTag*	xPT_content;
		static const cPlugTag*	xPT_closer;
		static const cPlugTag*	xPT_x;
		static const cPlugTag*	xPT_y;
		static const cPlugTag*	xPT_width;
		static const cPlugTag*	xPT_height;

		static const cCommand*	xLinkContent;
		static const cCommand*	xSetDim;	//!< Sets dimensions. 
		static const cCommand*	xSetCloser;	//!< Links a figment which is run when the close button is used.
		
		enum{
			eLinkContent = cFigment::eSwitchEnd + 1,
			eChangeDim,
			eLinkCloseFig,
			eSwitchEnd
		};

		static const dNatChar* identify(){ return "window frame"; }

		cWindowFrame();
		virtual ~cWindowFrame();

		virtual const dNatChar* name() const{ return identify(); }
		virtual dNameHash hash() const{ return tOutline<cWindowFrame>::hash(); }

		virtual void jack(ptrLead pLead, cContext *pCon);

	protected:
		tPlug<ptrFig>		mContent;	//!< Things to render in this window.
		tPlug<ptrFig>		mClosing;	//!< The object to run when this window is closed.
		tPlug<dUnitPix32>	mX, mY, mHeight, mWidth;

		//-------------------
		virtual void refreshDim(){}		//!< Sets dimensions of frame.
	};

}

#endif
