/*
 * !\file	linuxWorld.hpp
 * !\brief
 */

#ifndef LINUX_WORLD_HPP
#define LINUX_WORLD_HPP

#include <dlfcn.h>	// MUST include the dl library in whatever binary you build.
#include <sys/time.h> // for gettimeofday and timeval

#include "gt_base/world.hpp"

	//#define DYN_LIB_IMP_DEC(rnt) extern "C" rnt __stdcall


#define DYN_LIB_EXP_DEC(rnt) extern "C" rnt

#define DYN_LIB_DEF(rnt) rnt


namespace gt{

	//--------------------------------------------------------
	//!\class	cLinuxWorld
	//!\brief	Linux version of the abstract world class.
	class cLinuxWorld: public cWorld{
	public:
		cLinuxWorld();
		virtual ~cLinuxWorld();

		virtual dMillisec getAppTime();
		virtual void flushLines();

	protected:
		tAutoPtr<cWorld> makeWorld();
		void loop();
		void openAddon(const dStr &name);
		void closeAddon(const dStr &name);
		void getAddonList(dAddons &output);
		void readAddonCache(const dAddons &addons, dBlue2Addons &outMap, dAddon2Fresh &outFresh);
		void writeAddonCache(const dBlue2Addons &info);
		void getAddonNameFromFilename(const dPlaChar *filename, dStr *output);

	private:
		typedef std::map<dNameHash, void*> mapNameToHandle;
		typedef void (*draftFoo)(cWorld *pWorld);

		static const dPlaChar * ADDON_CACHE_FILE;
		static const dPlaChar * ADDON_PREFIX;
		static const dPlaChar * ADDON_POSTFIX;
		static const dPlaChar * LOG_FILE;

		timeval tempTime;
		mapNameToHandle mLibHandles;

		dMillisec getLinuxTime();
		void getDirContents(const dStr &dir, const dStr &search, bool dirsOnly, std::list<dStr> &output);
	};
}

#endif
