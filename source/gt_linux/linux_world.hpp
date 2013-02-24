/*
 * !\file	linuxWorld.hpp
 * !\brief
 */

#ifndef LINUX_WORLD_HPP
#define LINUX_WORLD_HPP

#include <dlfcn.h>	// MUST include the dl library in whatever binary you build.
#include <sys/time.h> // for gettimeofday and timeval

#include "gt_base/world.hpp"

namespace gt{

	//--------------------------------------------------------
	//!\class	cLinuxWorld
	//!\brief	Linux version of the abstract world class.
	class cLinuxWorld: public cWorld{
	public:
		cLinuxWorld();
		~cLinuxWorld();

		virtual dMillisec getAppTime();
		virtual void loop();
		virtual void flushLines();
		virtual void openAddon(const dStr &name);
		virtual void closeAddon(const dStr &name);

	private:
		typedef std::map<dNameHash, void*> mapNameToHandle;
		typedef void (*draftFoo)(cWorld *pWorld);

		static timeval tempTime;
		static dMillisec getLinuxTime();

		mapNameToHandle mLibHandles;
	};
}

#endif
