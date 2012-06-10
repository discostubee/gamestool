/*
 * !\file	postman.hpp
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
	//!\brief	The postman jacks into another figment using the lead it has been given. It does this
	//!			by adding the jack to the context's jack job list.
	class cPostman: public cFigment, private tOutline<cPostman>{
	public:
		static const cPlugTag *xPT_target;
		static const cPlugTag *xPT_command;
		static const cCommand::dUID xSetupPostman;

		static const dPlaChar* identify(){ return "postman"; }

		cPostman();
		virtual ~cPostman();

		virtual const dPlaChar* name() const{ return identify(); }
		virtual dNameHash hash() const{ return tOutline<cPostman>::hash(); }

		virtual void run(cContext* pCon);

	private:
		tPlug<ptrFig> mTarget;

		void patSetup(cLead *aLead);
	};
}

#endif
