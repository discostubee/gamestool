/*
 * !\file	thread.hpp
 * !\brief	Declares the threading figment. The strategy for threading figments is driven by the design of the context class.
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
 *
 */

#ifndef THREAD_HPP
#define THREAD_HPP

#include "figment.hpp"

namespace gt{

	//-------------------------------------------------------------------------------------
	//!\brief	Launches a new thread that runs through its linked figments. The context
	//!			is copied and run as a new one. When you run a thread figment, causes its
	//!			actual thread to run its loop again. The figment however, doesn't wait to
	//!			see if the thread finished or not. This is ideal for tasks which take an
	//!			unknown amount of time to finish.
	class cThread: public cFigment, public tOutline<cThread>{
	public:
		typedef boost::unique_lock<boost::mutex> dLock;

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
		bool firstRun;
		tPlug<ptrFig> link;
		boost::thread myThread;
		boost::mutex syncMu;
		boost::condition_variable sync;		//!< sync the thread to the calling of the run function.
		bool threadStop;					//!< Stops the thread loop.
		boost::mutex finishMu;				//!< The destructor must wait for the thread to finish.


		static void runThread(cThread *me, cContext* pCon);
	};
}

#endif
