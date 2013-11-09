/*
 * !\file	chainLink.hpp
 * !\brief
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

#ifndef CHAINLINK_HPP
#define CHAINLINK_HPP

#include <list>
#include "figment.hpp"

namespace gt{ //gamestool

	//-----------------------------------------------------------------------------------------------
	//!\brief	The base type of figment where you can add a link, which we use when we run, and
	//!\		adds to the links being passed back.
	class cChainLink: public cFigment{
	public:
		static const cPlugTag *xPT_link;
		static const cCommand::dUID xGetLink;
		static const cCommand::dUID xSetLink;

		cChainLink();
		virtual ~cChainLink();

		GT_IDENTIFY("chain link");
		GT_EXTENDS(cFigment);
		GT_VERSION(1);
		virtual dNameHash hash() const{ return getHash<cChainLink>(); }

		virtual void work(cContext* pCon);	//!< runs the linked figment.
		virtual void getLinks(std::list<ptrFig>* pOutLinks);	//!< Adds the link to the end of the list of other links.

	protected:
		tPlug<ptrFig> mLink;

		void patSetLink(ptrLead aLead);
		void patGetLink(ptrLead aLead);	//!< Gets only the linked figment.

		virtual void preLink();
		virtual void postLink();
	};
}

#endif
