/*
 * !\file	linuxWorld.hpp
 * !\brief
 */

#ifndef LINUX_WORLD_HPP
#define LINUX_WORLD_HPP

#include <sys/time.h> // for gettimeofday and timeval
#include <gt_base/figment.hpp>

namespace gt{

	//--------------------------------------------------------
	//!\class	cLinuxWorld
	//!\brief	Linux version of the abstract world class.
	class cLinuxWorld: public cWorld{
	private:
		static timeval tempTime;
		static dMillisec getLinuxTime();

	public:
		cLinuxWorld();
		~cLinuxWorld();

		virtual dMillisec getAppTime();
		virtual void loop();
		virtual void flushLines();
	};
}

#endif
