
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

		cPlug<ptrFig> stuff = gWorld->makeFig(makeHash("run list"));

		{
			ptrFig testAddon = gWorld->makeFig(getHash<cAddon>());
			ptrLead loadTestAddon = gWorld->makeLead(getHash<cAddon>(), cAddon::xLoadAddon->mID);
			cPlug<dStr> addonName;

			addonName.mD = "addonLogic";
			loadTestAddon->add(&addonName, cAddon::xPT_addonName);
			testAddon->jack(loadTestAddon);

			{
				cPlug<ptrFig> testLogic = gWorld->makeFig(makeHash("Logic input"));
				ptrLead addInput = gWorld->makeLead(makeHash("Logic input"), makeHash("add inputs"));
				cPlug<int> funNum;

				funNum = 1;
				addInput->add(&funNum, gWorld->getPlugTag(makeHash("Logic input"), makeHash("input lead")));
				testLogic.mD->jack(addInput);

				{
					ptrLead addStuff = gWorld->makeLead(getHash<cRunList>(), cRunList::xAdd->mID);
					cPlug<ptrFig> partyPooper = gWorld->makeFig(makeHash("world shutoff"));

					addStuff->addToPile(&testLogic);
					addStuff->addToPile(&partyPooper);

					stuff.mD->jack(addStuff);
				}
				
			}

			gWorld->setRoot(stuff.mD);
			gWorld->loop();
		}
	}
}

#endif
