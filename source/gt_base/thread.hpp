/*
 * !\file	thread.hpp
 * !\brief	Declares the threadding object.
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
 * 	    a
 * 	    b-->c
 * 	    b   c
 * 	|-->0   c
 * 	|   c<--c
 *  |<--c
 *
 *
 *
 *  a
 *  b-->c
 *  b   d-->e
 *  b   0   e--|
 *  b          |
 *  f-->g      |
 *  0   g<-----|
 *  0   g
 */

#ifndef THREAD_HPP
#define THREAD_HPP

#include "figment.hpp"

namespace gt{

	//-------------------------------------------------------------------------------------
	//!\brief	Launches a new thread that runs through its linked figment. The context
	//!			is copied and run as a new one. A figment is blocked from running when
	//!			it encounters a context from a different thread. But if it encounters
	//!			its own thread again, it unwinds. Uses a threadpool.
	class cThread: public cFigment, public tOutline<cThread>{
	public:
		static const cPlugTag*	xPT_fig;	//!< The figment to link.
		static const cCommand*	xLinkFig;	//!< Link figment to run in the separate thread.

		static const dNatChar* identify(){ return "thread"; }

		enum{
			eLinkFig = cFigment::eSwitchEnd +1,
			eSwitchEnd
		};

		cThread();
		virtual ~cThread();

		virtual const dNatChar* name() const{ return cThread::identify(); }
		virtual dNameHash hash() const{ return tOutline<cThread>::hash(); }
		virtual void run(cContext* pCon);
		virtual void jack(ptrLead pLead, cContext* pCon);

	protected:
		tPlug<ptrFig> link;
	};
}

#endif
