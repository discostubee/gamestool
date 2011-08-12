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

	gWorld = new cOSXWorld();

	tOutline<cRunList>::draft();
	tOutline<cFigment>::draft();
	tOutline<cAddon>::draft();
	tOutline<cAddon_OSX>::draft();

	gWorld->flushLines();
#ifdef GTUT_GOOGLE
	::testing::InitGoogleTest(&argc, argv);
	result = RUN_ALL_TESTS();
	gWorld->flushLines();
#endif

	delete gt::gWorld;

	std::cout << "Tests over." << std::endl;

	return result;
}

#ifdef GTUT

	GTUT_START(OSX, load){
		ptrFig elements;
		cPlug<ptrFig> addonGL;
		cPlug<ptrFig> window;
		ptrLead setPath = gWorld->makeLead(getHash<cAddon>(), cAddon::xLoadAddon->mID);
		cPlug<dStr> path = cPlug<dStr>("X11GL");
		ptrLead addElements = gWorld->makeLead(getHash<cRunList>(), cRunList::xAdd->mID);

		elements = gWorld->makeFig(getHash<cRunList>());

		addonGL.mD = gWorld->makeFig(getHash<cAddon>());
		setPath->add(&path, cAddon::xPT_addonName);
		addonGL.mD->jack(setPath);

		window.mD = gWorld->makeFig(getHash<cWindowFrame>());

		addElements->addToPile(&addonGL);
		addElements->addToPile(&window);
		elements->jack(addElements);

		gWorld->setRoot(elements);
	}GTUT_END;

	GTUT_START(OSX, unload){
		bool addonBlueprintRemoved = false;

		gWorld->setRoot(gWorld->getEmptyFig());	// Will take out everything.
		try{
			gWorld->getBlueprint(getHash<cAddon_OSX>());
		}catch(excep::base_error){
			DBUG_LO("removed the OSX addon blueprint.");
			addonBlueprintRemoved = true;
		}
		try{
			gWorld->getBlueprint(getHash<cWindowFrame>());
		}catch(excep::base_error){
			DBUG_LO("removed the x11 window frame blueprint.");
			addonBlueprintRemoved = true;
		}
		GTUT_ASRT(addonBlueprintRemoved, "didn't remove blueprints.");

		TRYME(gWorld->getBlueprint(getHash<cAddon>()));	// Check the base addon was restored.
	}GTUT_END;

#endif
