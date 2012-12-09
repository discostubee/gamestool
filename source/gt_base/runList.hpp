/*
 * !\file	runList.hpp
 * !\brief	contains the runlist declaration.
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

#ifndef RUNLIST_HPP
#define RUNLIST_HPP

#include <list>
#include "figment.hpp"
#include "plugContainer.hpp"

namespace gt{ //gamestool

	//-----------------------------------------------------------------------------------------------
	//!\brief	When you run this figment, you also run every figment in its list.
	class cRunList: public cFigment{
	public:
		static const cCommand::dUID	xAdd;

		cRunList();
		virtual ~cRunList();

		//- Required
		GT_IDENTIFY("run list");
		GT_EXTENDS(cFigment);
		GT_VERSION(1);
		virtual dNameHash hash() const{ return getHash<cRunList>(); }

		//- Optional
		virtual void work(cContext* pCon);	//!< runs every element in the list
		virtual void getLinks(std::list<ptrFig>* pOutLinks);

	protected:
		typedef tPlugArray<ptrFig> dList;

		dList mList;

		void patAdd(ptrLead aLead);
	};

	//-----------------------------------------------------------------------------------------------
	//!\brief Running or not running an object is controlled by this valve station. Use the jack to
	//!		change what objects will be run every time this figment is run.
	//!\todo	Save and Load.
	class cValves: public cRunList{
	protected:
		tPlugMap<size_t, bool> mStates;	//!< Maps states to the list entries. These states control which objects are run.

	public:
		static const cPlugTag*	xPT_state;		//!< Turn turns valve on.
		static const cPlugTag*	xPT_valveIdx;	//!< The numeric index for the valve.

		static const cCommand::dUID	xSetState;

		cValves();
		virtual ~cValves();

		GT_IDENTIFY("valves");
		GT_EXTENDS(cRunList);
		GT_VERSION(1);
		virtual dNameHash hash() const { return getHash<cValves>(); }

		//- Optional
		virtual void work(cContext* pCon);				//!< runs every element in the list

	protected:
		void patSetValve(ptrLead pLead);
	};
}

#endif
