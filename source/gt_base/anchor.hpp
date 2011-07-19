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
	//!\todo	Rename, because we are not dealing with a chain, we are dealing with a network (which
	//!			is not the same as dealing with a single direction tree).
	class cAnchor: public cFigment, private tOutline<cAnchor>{
	public:
		static const tPlugTag*	xPT_root;
		static const cCommand*	xSetRoot;	//!\todo	Rename to linkRoot. Gotta keep the term link consistent.
		static const cCommand*	xGetRoot;

		enum{
			eSetRoot = cFigment::eSwitchEnd,
			eGetRoot,
			eSwitchEnd,
		};

		cAnchor();
		virtual ~cAnchor();

		//- Required
		static const dNatChar* identify(){ return "anchor"; }
		virtual const dNatChar* name() const { return identify(); }	//!<
		virtual dNameHash hash() const { return tOutline<cAnchor>::hash(); }

		//- Optional
		virtual void run(cContext* pCon);
		virtual void jack(ptrLead pLead, cContext* pCon);
		virtual void save(cByteBuffer* pAddHereb);
		virtual void loadEat(cByteBuffer* pBuff, dReloadMap* pReloads = NULL);

	private:
		tPlug<ptrFig>	mRoot;
	};
}

#endif
