//!\file	main.cpp
//!\brief	Aims to test the world's ability to load addons, print lines and profile tokens.
//!\note	You need the  OSX GL addon.

// Include the stuff we want to test.

#if defined(__APPLE__)
	#include "gt_OSX/OSX_world.hpp"
	#include "gt_OSX/OSX_addon.hpp"
#elif defined(linux)
	#include "gt_linux/linux_world.hpp"
	#include "gt_linux/linux_addon.hpp"
#endif

#include "gt_base/runList.hpp"
#include "gt_graphics/windowFrame.hpp"

#include <fstream>

using namespace gt;

int
main(int argc, char **argv){
	int result = EXIT_SUCCESS;
	std::cout
		<< "Running gamestool tests "
#if defined(__APPLE__)
		<< " for mac. "
#elif defined(linux)
		<< " for linux. "
#endif
		<< "Version 0.2. "
#ifdef GT_THREADS
		<< "Threadding. "
#endif
		<< std::endl;

#if defined(__APPLE__)
	gWorld.take( new cOSXWorld() );
#elif defined(linux)
	gWorld.take( new cLinuxWorld() );
#endif

	tOutline<cRunList>::draft();
	tOutline<cFigment>::draft();
	tOutline<cAddon>::draft();

#if defined(__APPLE__)
	tOutline<cAddon_OSX>::draft();
#elif defined(linux)
	tOutline<cAddon_linux>::draft();
#endif

	gWorld.get()->flushLines();
#ifdef GTUT_GOOGLE
	::testing::InitGoogleTest(&argc, argv);
	result = RUN_ALL_TESTS();
#endif

	{
		std::ofstream profileReport("profile_report.txt");
		gt::gWorld.get()->makeProfileReport(profileReport);//(std::cout);
		profileReport.close();
	}


	gt::gWorld.cleanup();

	return result;
}

#ifdef GTUT
namespace gt{
	GTUT_START(testLinux, load){
		cContext fake;
		ptrFig elements;
		tPlug<ptrFig> addonGL;
		tPlug<ptrFig> window;
		ptrLead setPath = gWorld.get()->makeLead(cAddon::xLoadAddon, fake.getSig());
		tPlug<dStr> path("X11GL");
		ptrLead addElements = gWorld.get()->makeLead(cRunList::xAdd, fake.getSig());

		elements = gWorld.get()->makeFig(getHash<cRunList>());

		addonGL.mD = gWorld.get()->makeFig(getHash<cAddon>());
		setPath->addPlug(&path, cAddon::xPT_addonName);
		addonGL.mD->jack(setPath, &fake);

		window.mD = gWorld.get()->makeFig(getHash<cWindowFrame>());

		addElements->addToPile(&addonGL);
		addElements->addToPile(&window);
		elements->jack(addElements, &fake);

		gWorld.get()->setRoot(elements);
	}GTUT_END;

	GTUT_START(testLinux, unload){
		bool addonBlueprintRemoved = false;

		gWorld.get()->setRoot(gWorld.get()->getEmptyFig());	// Will take out everything.
		try{
			gWorld.get()->getBlueprint(getHash<cAddon_linux>());
		}catch(excep::base_error){
			DBUG_LO("removed the OSX addon blueprint.");
			addonBlueprintRemoved = true;
		}
		try{
			gWorld.get()->getBlueprint(getHash<cWindowFrame>());
		}catch(excep::base_error){
			DBUG_LO("removed the x11 window frame blueprint.");
			addonBlueprintRemoved = true;
		}
		GTUT_ASRT(addonBlueprintRemoved, "didn't remove blueprints.");

		TRYME(gWorld.get()->getBlueprint(getHash<cAddon>()));	// Check the base addon was restored.
	}GTUT_END;
}
#endif
