/*
 * !\file	thread_linux.hpp
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
 */

#ifndef THREAD_LINUX_HPP
#define THREAD_LINUX_HPP

#include <gt_base/thread.hpp>

namespace gt{

	//-------------------------------------------------------------------------------------
	class cLinux_thread: public cThread, private tOutline<cLinux_thread>{
	public:
		static const char* identify(){ return "figment"; }
		static dNameHash replaces(){ return getHash<cThread>(); }

		//-----------------------------
		cLinux_thread();
		virtual ~cLinux_thread();

		//-----------------------------
		virtual const char* name() const{ return identify(); }		//!< Virtual version of identify.
		virtual dNameHash hash() const{ return tOutline<cLinux_thread>::hash(); }

		//-----------------------------
		virtual void run(cContext* pCon);		//!<
	};
}

#endif
