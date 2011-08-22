
#ifndef GRAPHICS_EXAMPLE_HPP
#define GRAPHICS_EXAMPLE_HPP

#if defined(__APPLE__)
	#include <gt_OSX/OSX_addon.hpp>
#elif defined(LINUX)
	#include <gt_linux/linux_addon.hpp>
#elif defined(WIN32)
	#include <gt_win7/win_addon.hpp>
#endif

#include <gt_base/addon.hpp>
#include <gt_base/runList.hpp>

namespace showoff{

inline void graphics(){
	using namespace gt;

	PROFILE;
	DBUG_LO("showing off addons.");

	//- Need to draft all the objects we want to use first:
	tOutline<cFigment>::draft();
	tOutline<cEmptyFig>::draft();
	tOutline<cWorldShutoff>::draft();
	tOutline<cAddon>::draft();
	tOutline<cRunList>::draft();

#if defined(__APPLE__)
	tOutline<cAddon_OSX>::draft();
#elif defined(LINUX)
	tOutline<cAddon_linux>::draft();
#elif	defined(WIN32)
	tOutline<cAddon_win>::draft();
#endif

	{
		ptrFig prettyAddon = gWorld.get()->makeFig(getHash<cAddon>());
		cContext fake;

		{
			ptrLead loadAddon = gWorld.get()->makeLead(getHash<cAddon>(), cAddon::xLoadAddon->mID, &fake);
			tPlug<dStr> addonName;

		#if		defined	__APPLE__
			addonName.mD = "X11GL";
		#elif	defined	LINUX
			addonName.mD = "X11GL";
		#elif	defined	WIN32
			addonName.mD = "WinGL";
		#endif

			loadAddon->add(&addonName, cAddon::xPT_addonName, &fake);
			prettyAddon->jack(loadAddon, &fake);
		}
		{
			tPlug<ptrFig> stuff = gWorld.get()->makeFig(makeHash("run list"));
			tPlug<ptrFig> shiney = gWorld.get()->makeFig(makeHash("window frame"));

			{
				tPlug<ptrFig> partyPooper = gWorld.get()->makeFig(makeHash("world shutoff"));
				ptrLead setCloser = gWorld.get()->makeLead(makeHash("window frame"), makeHash("link closer"), &fake);

				setCloser->add(
					&partyPooper,
					gWorld.get()->getPlugTag(makeHash("window frame"), makeHash("closer")),
					&fake
				);

				shiney.mD->jack(setCloser, &fake);
			}
			{
				ptrLead addStuff = gWorld.get()->makeLead(getHash<cRunList>(), cRunList::xAdd->mID, &fake);

				addStuff->addToPile(&shiney, &fake);

				stuff.mD->jack(addStuff, &fake);
			}

			gWorld.get()->setRoot(stuff.mD);
			gWorld.get()->loop();
		}
		gWorld.get()->setRoot(gWorld.get()->getEmptyFig());
	}
}

}

#endif
