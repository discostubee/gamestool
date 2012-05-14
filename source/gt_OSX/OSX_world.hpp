/*
 * !\file	OSX_World.hpp
 * !\brief
 */

#ifndef OSX_WORLD_HPP
#define OSX_WORLD_HPP

#include "gt_base/figment.hpp"

#include <sys/time.h> // for gettimeofday and timeval

namespace gt{

	//--------------------------------------------------------
	//!\brief	The Apple Mac version of the abstract world class.
	class cOSXWorld: public cWorld{
	public:
		cOSXWorld();
		~cOSXWorld();

		virtual dMillisec getAppTime();
		virtual void loop();
		virtual void flushLines();

	private:
		static timeval tempTime;
		static dMillisec getOSXTime();
	};
}

#endif
