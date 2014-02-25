/*
 * !\file	figFactory.hpp
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

#ifndef FIGFACTORY_HPP
#define FIGFACTORY_HPP

#include "figment.hpp"


/////////////////////////////////////////////////////////////////////////////////////
// classes
namespace gt{

	//-------------------------------------------------------------------------------------
	//!\brief	Makes figments and opens addons. It also provides lists of possible figments
	//!			and addons.
	class cFigFactory: public cFigment{
	public:
		static const cPlugTag* xPT_figHash;	//!< The hash of the figment we want.
		static const cPlugTag* xPT_newFig;	//!< The plug to put the new figment in.
		static const cPlugTag* xPT_catalog;	//!<

		static const cCommand::dUID	xMakeFig;		//!< Makes a new fig of the given hash, and puts it into the provided plug.
		static const cCommand::dUID	xGetAddons;		//!< Gives a list of the addons.
		static const cCommand::dUID	xGetFigCatalog;	//!< Creates a list of all the available figments as 2 plug containers: names and hashes.

		cFigFactory();
		virtual ~cFigFactory();

		GT_EXTENDS(cFigment)
		GT_IDENTIFY("figment factory");
		virtual dNameHash hash() const{ return getHash<cFigFactory>(); }

	protected:
		void patMakeFig(ptrLead aLead);
		void patGetFigCatalog(ptrLead aLead);
	};

}


#endif
