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

#include "chainLink.hpp"

namespace gt{ //gamestool

	//-----------------------------------------------------------------------------------------------
	//!\class	cAnchor
	//!\brief	Yarr, yea call that an anchor! An anchor can save or load all the figments it links too
	//!			into/out of a buffer
	//!\note	Because stuff is joined together like links in a chain, it makes sense to think of the
	//!			object which forms the root of the chain as the anchor.
	class cAnchor: public cChainLink{
	public:

		cAnchor();
		virtual ~cAnchor();

		GT_IDENTIFY("anchor");
		GT_EXTENDS(cChainLink);
		virtual dNameHash hash() const { return getHash<cAnchor>(); }

		virtual void save(cByteBuffer* pAddHereb);	//!< Performs special saving.
		virtual void loadEat(cByteBuffer* pBuff, dReloadMap* pReloads = NULL);	//!< Performs special loading.
	};
}

#endif
