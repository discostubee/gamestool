/*
 * !\file	anchor.hpp
 * !\brief	Contains the declaration of the anchor class
 */

#ifndef	ANCHOR_HPP
#define ANCHOR_HPP

#include "fileIO.hpp"

namespace gt{ //gamestool

	//-----------------------------------------------------------------------------------------------
	//!\class	cAnchor
	//!\brief	An anchor can save or load all the figments it links too into/out of a buffer
	//!\note	Because stuff is joined together like links in a chain, it makes sense to think of the
	//!			object which forms the root of the chain as the anchor.
	class cAnchor: public cFigment, private tOutline<cAnchor>{
	private:

		cPlug<ptrFig>	mRoot;

	public:
		static const cPlugTag*	xPT_root;
		static const cCommand*	xSetRoot;
		static const cCommand*	xGetRoot;

		enum{
			eSetRoot = cFigment::eSwitchEnd,
			eGetRoot,
			eSwitchEnd,
		};

		static const dNatChar* identify(){ return "anchor"; }
		static void draft(){ tOutline<cAnchor>::draft(); }
		static void requirements();

		cAnchor();
		virtual ~cAnchor();

		virtual const dNatChar* name() const { return identify(); }	//!<
		virtual dNameHash hash() const { return tOutline<cAnchor>::hash(); }

		virtual void run(cContext* pCon);
		virtual void jack(ptrLead pLead);
		virtual cByteBuffer& save();
		virtual void loadEat(cByteBuffer* pBuff, dReloadMap* pReloads = NULL);
	};
}

#endif
