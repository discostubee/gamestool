/*
 * !\file	postman.hpp
 * !\brief
 *
 *  Copyright (C) 2010  Stuart Bridgens
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * EXAMPLE
 * Dealt with because when c encounters c again, it unwinds. b gets blocked
 * until c winds.
 *
 */

#ifndef POSTMAN_HPP
#define POSTMAN_HPP

#include "figment.hpp"

namespace gt{

	//-------------------------------------------------------------------------------------
	//!\brief	The postman jacks into another figment using the lead it has been given.
	class cPostman: public cFigment, private tOutline<cPostman>{
	public:
		static const cPlugTag* xPT_target;
		static const cPlugTag* xPT_lead;
		static const cCommand* xSetupPostman;

		static const dNatChar* identify(){ return "postman"; }

		enum{
			eSetup = cFigment::eSwitchEnd +1,
			eSwitchEnd
		};

		cPostman();
		virtual ~cPostman();

		virtual const dNatChar* name() const{ return identify(); }
		virtual dNameHash hash() const{ return tOutline<cPostman>::hash(); }

		virtual void run(cContext* pCon);
		virtual void jack(ptrLead pLead, cContext* pCon);
	private:
		tPlug<ptrLead> mLead;
		tPlug<ptrFig> mTarget;
	};
}

#endif
