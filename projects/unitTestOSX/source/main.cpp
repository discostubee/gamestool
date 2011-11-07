//!\file	main.cpp
//!\brief	Aims to test the world's ability to load addons, print lines and profile tokens.
//!\note	You need the  OSX GL addon.

// Include the stuff we want to test.
#include <gt_base/runList.hpp>
#include <gt_OSX/OSX_world.hpp>
#include <gt_OSX/OSX_addon.hpp>
#include <gt_graphics/windowFrame.hpp>

using namespace gt;

int
main(int argc, char **argv){
	int result = EXIT_SUCCESS;
	std::cout << "Running gamestool tests for OSX. Version 0.1" << std::endl;

	gWorld.take( new cOSXWorld() );

	tOutline<cRunList>::draft();
	tOutline<cFigment>::draft();
	tOutline<cAddon>::draft();
	tOutline<cAddon_OSX>::draft();

	gWorld.get()->flushLines();
#ifdef GTUT_GOOGLE
	::testing::InitGoogleTest(&argc, argv);
	result = RUN_ALL_TESTS();
#endif

	gt::gWorld.get()->makeProfileReport(std::cout);
	gt::gWorld.cleanup();

	return result;
}

#ifdef GTUT
namespace gt{
	GTUT_START(testOSX, load){
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

	GTUT_START(testOSX, unload){
		bool addonBlueprintRemoved = false;

		gWorld.get()->setRoot(gWorld.get()->getEmptyFig());	// Will take out everything.
		try{
			gWorld.get()->getBlueprint(getHash<cAddon_OSX>());
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
