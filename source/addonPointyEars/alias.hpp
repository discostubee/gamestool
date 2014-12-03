/*
 * !\file	alias.hpp
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
 *
 *
 */

#ifndef REFLECTION_HPP
#define REFLECTION_HPP

#include "gt_base/figment.hpp"

namespace gt{

	//!\brief	Any jack or run commands are passed to the bound figment instead of the alias.
	//!			The cool stuff happens when you get to change the alias name (which also changes the hash). This means that when you search for a figment
	//!			in the context, you can look for an alias instead of the figment type name.
	//!\note	Identify always returns 'alias'.
	class cAlias : public cFigment{
	public:
		static const cPlugTag *xPT_fig;
		static const cPlugTag *xPT_alias;
		static const cCommand::dUID xBindFig;
		static const cCommand::dUID xSetAlias;

		cAlias();
		virtual ~cAlias();

		static const dPlaChar* identify(){ return "alias"; }

		GT_EXTENDS(cFigment);
		GT_VERSION(1);

		virtual const dPlaChar* name() const;	//!< Returns the name of the bound figment, or "alias" by default.
		virtual dNameHash hash() const;	//!< Returns a hash of the current alias.
		virtual void loadEat(cByteBuffer* pBuff, dReloadMap *aReloads = NULL);	//!< Recreates the hash, if a name was saved.
		virtual void getLinks(std::list<ptrFig>* pOutLinks);
		virtual void work(cContext* pCon);
		virtual void jack(ptrLead pLead, cContext* pCon);

	private:
		tPlug<ptrFig> mBound;	//!< The figment this alias is bound to.
		tPlug<dStr> mAName;	//!< Alias name.
		dNameHash mAHash;	//!< Hash of alias name.

		void patBindFig(ptrLead aLead);
		void patSetAlias(ptrLead aLead);
	};

}

#endif
