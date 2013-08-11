/*
 * !\file	OSX_World.hpp
 * !\brief
 */

#ifndef OSX_WORLD_HPP
#define OSX_WORLD_HPP

#include "gt_base/figment.hpp"

#include <sys/time.h> // for gettimeofday and timeval
#include <dlfcn.h>	// MUST include the dl library in whatever binary you build.

namespace gt{

	//--------------------------------------------------------
	//!\brief	The Apple Mac version of the abstract world class.
	class cOSXWorld: public cWorld{
	public:
		cOSXWorld();
		virtual ~cOSXWorld();

		virtual dMillisec getAppTime();
		virtual void loop();
		virtual void flushLines();
		virtual void openAddon(const dStr &name);

	protected:
		virtual void closeAddon(const dStr &name);

	private:
		typedef std::map<dNameHash, void*> mapNameToHandle;
		typedef void (*draftFoo)(cWorld *pWorld);

		static timeval tempTime;
		static dMillisec getOSXTime();

		mapNameToHandle mLibHandles;
	};
}


#endif
