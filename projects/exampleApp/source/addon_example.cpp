
#ifndef ADDON_EXAMPLE_HPP
#define ADDON_EXAMPLE_HPP

#if defined LINUX
	#include <gt_linux/linux_addon.hpp>
#elif defined WIN32
	#include <gt_win7/win_addon.hpp>
#endif

#include <gt_base/addon.hpp>
#include <gt_base/runList.hpp>

namespace showoff{

	inline void addons(){
		using namespace gt;

		PROFILE;
		DBUG_LO("showing off addons.");

		cContext fake;

		//- Need to draft all the objects we want to use first:
		tOutline<cRunList>::draft();
		tOutline<cFigment>::draft();
		tOutline<cEmptyFig>::draft();
		tOutline<cWorldShutoff>::draft();
		tOutline<cAddon>::draft();

		#if defined LINUX
			tOutline<cAddon_linux>::draft();
		#elif defined WIN32
			tOutline<cAddon_win>::draft();
		#endif

		tPlug<ptrFig> stuff = gWorld.get()->makeFig(makeHash("run list"));

		{
			ptrFig testAddon = gWorld.get()->makeFig(getHash<cAddon>());
			ptrLead loadTestAddon = gWorld.get()->makeLead(getHash<cAddon>(), cAddon::xLoadAddon->mID, &fake);
			tPlug<dStr> addonName;


			gWorld.get()->setRoot(stuff.mD);
			gWorld.get()->loop();
		}
	}
}

#endif
