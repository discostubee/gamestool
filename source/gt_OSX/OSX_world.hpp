/*
 * !\file	OSX_World.hpp
 * !\brief
 */

#ifndef OSX_WORLD_HPP
#define OSX_WORLD_HPP

#include <sys/time.h> // for gettimeofday and timeval
#include <gt_base/figment.hpp>

namespace gt{

	//--------------------------------------------------------
	//!\brief	The Apple Mac version of the abstract world class.
	class cOSXWorld: public cWorld{
	private:
		static timeval tempTime;
		static dMillisec getOSXTime();

	public:
		cOSXWorld();
		~cOSXWorld();

		virtual dMillisec getAppTime();
		virtual void loop();
		virtual void flushLines();
	};
}

#endif
