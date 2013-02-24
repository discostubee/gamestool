/*
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

#ifndef VALVE_HPP
#define VALVE_HPP

#include "figment.hpp"

namespace gt{

	//!\brief	Runs the linked figment, but only if its valve is open.
	//!\note	Valve is open when made.
	class cValve: public cFigment, private tOutline<cValve>{
	public:
		static const cPlugTag* xPT_link;
		static const cPlugTag* xPT_state;
		static const cCommand::dUID xSetLink;
		static const cCommand::dUID xSetState;
		static const cCommand::dUID xGetState;

		cValve();
		virtual ~cValve();

		GT_IDENTIFY("valve");
		GT_EXTENDS(cFigment);
		GT_VERSION(1);
		virtual dNameHash hash() const{ return getHash<cValve>(); }

		virtual void getLinks(std::list<ptrFig>* pOutLinks);

	protected:
		void patSetLink(ptrLead aLead);
		void patSetState(ptrLead aLead);
		void patGetState(ptrLead aLead);

	private:
		tPlug<ptrFig> mLink;
		tPlug<bool> mState;
	};

}


#endif
