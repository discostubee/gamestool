/*
 * !\file	anchor.hpp
 * !\brief	Contains the declaration of the anchor class
 *
 **********************************************************************************************************
 *  Copyright (C) 2010  Stuart Bridgens
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License (version 3) as published by
 *  the Free Software Foundation.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *********************************************************************************************************
 */

#ifndef	ANCHOR_HPP
#define ANCHOR_HPP

#include "fileIO.hpp"

namespace gt{ //gamestool

	//-----------------------------------------------------------------------------------------------
	//!\class	cAnchor
	//!\brief	Yarr, yea call that an anchor! An anchor can save or load all the figments it links too into/out of a buffer
	//!\note	Because stuff is joined together like links in a chain, it makes sense to think of the
	//!			object which forms the root of the chain as the anchor.
	//!\todo	Rename, because we are not dealing with a chain, we are dealing with a network (which
	//!			is not the same as dealing with a single direction tree).
	class cAnchor: public cFigment, private tOutline<cAnchor>{
	public:
		static const cPlugTag*	xPT_root;
		static const cCommand::dUID	xSetRoot;	//!\todo	Rename to linkRoot. Gotta keep the term link consistent.
		static const cCommand::dUID	xGetRoot;

		cAnchor();
		virtual ~cAnchor();

		//- Required
		static const dNatChar* identify(){ return "anchor"; }
		virtual const dNatChar* name() const { return identify(); }	//!<
		virtual dNameHash hash() const { return tOutline<cAnchor>::hash(); }
		static dNameHash extends(){ return getHash<cFigment>(); }
		virtual dNameHash getExtension() const { return extends(); }

		//- Optional
		virtual void run(cContext* pCon);
		virtual void save(cByteBuffer* pAddHereb);
		virtual void loadEat(cByteBuffer* pBuff, dReloadMap* pReloads = NULL);
		virtual void getLinks(std::list<ptrFig>* pOutLinks);

	protected:
		void patSetRoot(ptrLead aLead);
		void patGetRoot(ptrLead aLead);

	private:
		tPlug<ptrFig>	mRoot;
	};
}

#endif
