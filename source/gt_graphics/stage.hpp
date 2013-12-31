/*
 * !\file	stage.hpp
 * !\brief
 */

#ifndef STAGE_HPP
#define STAGE_HPP

#include "gt_base/chainLink.hpp"
#include "primitives.hpp"

namespace gt{
	//!\class	cStage
	//!\brief	Provides a typical window interface like what we see in most OSs.
	//!\note	Treats different screens as being a certain distance from the primary one. Some devices with screens at set distances (IE, a DS)
	//!			Can have stage positions that counts the area between screen.
	//!			 ___
	//!			|  _|_      _       _
	//!			|_|   | => | |\ => |_|<|  =>
	//!			  |___|    |_|-
	//!			  stage    film   camera
	class cStage: public cChainLink{
	public:
		static const cPlugTag*	xPT_closer;
		static const cPlugTag*	xPT_layout;
		static const cPlugTag*	xPT_fullscreen;

		static const cCommand::dUID	xLinkCloser;	//!< Links a figment which is run when the OS tells the window to close.
		static const cCommand::dUID	xSetLayout;		//!<
		static const cCommand::dUID xGetLayout;	//!< Get the pixel dimensions and position of the window.
		static const cCommand::dUID xSetFullscreen;	//!< Goes into or out of fullscreen.

		cStage();
		virtual ~cStage();

		GT_IDENTIFY("stage");
		GT_EXTENDS(cChainLink);
		GT_VERSION(1);
		virtual dNameHash hash() const{ return getHash<cStage>(); }

		virtual void getLinks(std::list<ptrFig>* pOutLinks);

	protected:
		tPlug<ptrFig> mCloser;	//!< The object to run when this window is closed.
		tPlug< shape::tRectangle<dUnitPix> > mLayout;

		void patLinkCloser(ptrLead aLead);
		void patSetLayout(ptrLead aLead);
		void patGetLayout(ptrLead aLead);
		void patSetFullscreen(ptrLead aLead);

		virtual void refreshDim() { DONT_USE_THIS; } //!< Takes the current layout and applies it.
		virtual void setFullscreen(bool pFullescreen) { DONT_USE_THIS; }
	};

}

#endif
