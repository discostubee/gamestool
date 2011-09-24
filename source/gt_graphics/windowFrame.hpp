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

		static const cCommand::dUID	xLinkContent;
		static const cCommand::dUID	xSetDim;	//!< Sets dimensions.
		static const cCommand::dUID	xSetCloser;	//!< Links a figment which is run when the OS tells the window to close.


		cWindowFrame();
		virtual ~cWindowFrame();

		//----- Stuff we must have.
		static const dNatChar* identify(){ return "window frame"; }
		virtual const dNatChar* name() const{ return identify(); }
		virtual dNameHash hash() const{ return tOutline<cWindowFrame>::hash(); }

	protected:
		tPlug<ptrFig>		mContent;	//!< Things to render in this window.
		tPlug<ptrFig>		mClosing;	//!< The object to run when this window is closed.
		tPlug<dUnitPix32>	mX, mY, mHeight, mWidth;

		void patLink(cLead *aLead);
		void patSetDim(cLead *aLead);
		void patSetCloser(cLead *aLead);

		//----- Stuff to override.
		virtual void refreshDim(){}		//!< Sets dimensions of the frame.
	};

}

#endif
