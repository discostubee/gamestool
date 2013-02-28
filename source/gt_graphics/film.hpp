/*
 * !\file	layer.hpp
 * !\brief
 */

#ifndef LAYER_HPP
#define LAYER_HPP

#include "stage.hpp"

namespace gt{

	//!\brief	Film is where we render to. Its resolution is dictated by other figments like the stage or a texture.
	class cFilm: public cFigment{
	public:
		static const cPlugTag*	xPT_content;
		static const cPlugTag*	xPT_layout;			//!< Rectangle that represents where the film appears on a stage or texture.
		static const cPlugTag*	xPT_rez;			//!<

		static const cCommand::dUID	xLinkContent;	//!<
		static const cCommand::dUID xSetLayout;		//!<
		static const cCommand::dUID xGetLayout;		//!<
		static const cCommand::dUID xGetRez;		//!< Gets the resolution (number of pixels high and wide) for this film.

		cFilm();
		virtual ~cFilm();

		GT_IDENTIFY("film");
		GT_EXTENDS(cFigment);
		GT_VERSION(1);
		virtual dNameHash hash() const { return getHash<cFilm>(); }

		virtual void getLinks(std::list<ptrFig> *pOutLinks);

	protected:
		tPlug<ptrFig> mLink;		//!< Run everything linked by this root.
		tPlug< shape::rectangle<dUnitPix> > mLayout;

		void patSetLink(ptrLead aLead);
		void patSetLayout(ptrLead aLead);
		void patGetLayout(ptrLead aLead);
		void patGetRez(ptrLead aLead);

		virtual void updateLayout() {}
		virtual void getRez(tDim2D<dUnitPix> &aSetMe) {}
	};
}
#endif

