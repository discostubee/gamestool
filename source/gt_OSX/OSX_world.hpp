/*
 * !\file	OSX_World.hpp
 * !\brief
 */

#ifndef OSX_WORLD_HPP
#define OSX_WORLD_HPP

#include "gt_base/figment.hpp"

#include <sys/time.h> // for gettimeofday and timeval
#include <dlfcn.h>	// MUST include the dl library in whatever binary you build.

	//#define DYN_LIB_IMP_DEC(rnt) extern "C" rnt __stdcall
#define DYN_LIB_EXP_DEC(rnt) extern "C" rnt
#define DYN_LIB_DEF(rnt) rnt

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

	protected:
		tAutoPtr<cWorld> makeWorld();
		void openAddon(const dStr &name);
		void closeAddon(const dStr &name);
		void getAddonList(dAddons &output);
		void readAddonCache(const dAddons &addons, dBlue2Addons &outMap, dAddon2Fresh &outFresh);
		void writeAddonCache(const dBlue2Addons &info);

	private:
		typedef std::map<dNameHash, void*> mapNameToHandle;
		typedef void (*draftFoo)(cWorld *pWorld);

		static const char * ADDON_POSTFIX;
		static const char * ADDON_CACHE_FILE;
		static const char * LOG_FILE;

		static timeval tempTime;

		static dMillisec getOSXTime();
		static std::vector<dStr> getFiles(const dStr &search);

		mapNameToHandle mLibHandles;
	};
}


#endif
