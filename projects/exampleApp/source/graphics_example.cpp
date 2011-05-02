
#ifndef GRAPHICS_EXAMPLE_HPP
#define GRAPHICS_EXAMPLE_HPP

#if defined(LINUX)
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

#ifdef	LINUX
	tOutline<cAddon_linux>::draft();
#elif	WIN32
	tOutline<cAddon_win>::draft();
#endif

	{
		ptrFig prettyAddon = gWorld->makeFig(getHash<cAddon>());

		{
			ptrLead loadAddon = gWorld->makeLead(getHash<cAddon>(), cAddon::xLoadAddon->mID);
			cPlug<dStr> addonName;

		#ifdef	LINUX
			addonName.mD = "addonX11GL";
		#elif	WIN32
			addonName.mD = "addonWinGL";
		#endif

			loadAddon->add(&addonName, cAddon::xPT_addonName);
			prettyAddon->jack(loadAddon);
		}
		{
			cPlug<ptrFig> stuff = gWorld->makeFig(makeHash("run list"));
			cPlug<ptrFig> shiney = gWorld->makeFig(makeHash("window frame"));

			{
				cPlug<ptrFig> partyPooper = gWorld->makeFig(makeHash("world shutoff"));
				ptrLead setCloser = gWorld->makeLead(makeHash("window frame"), makeHash("link closer"));

				setCloser->add(
					&partyPooper,
					gWorld->getPlugTag(makeHash("window frame"), makeHash("closer"))
				);

				shiney.mD->jack(setCloser);
			}
			{
				ptrLead addStuff = gWorld->makeLead(getHash<cRunList>(), cRunList::xAdd->mID);

				addStuff->addToPile(&shiney);

				stuff.mD->jack(addStuff);
			}

			gWorld->setRoot(stuff.mD);
			gWorld->loop();
		}
	}
}

}

#endif
