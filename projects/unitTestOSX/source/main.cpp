//!\file	main.cpp
//!\brief	Aims to test the world's ability to load addons, print lines and profile tokens.
//!\note	You need the  OSX GL addon.

// Include the stuff we want to test.
#include <gt_OSX/OSX_world.hpp>
#include <gt_OSX/OSX_addon.hpp>
#include <gt_graphics/windowFrame.hpp>

using namespace gt;

ptrFig addonGL;

int
main(int argc, char **argv){
	int result = EXIT_SUCCESS;
	std::cout << "Running gamestool tests for OSX. Version 0.1" << std::endl;

	gWorld = new cOSXWorld();

	tOutline<cFigment>::draft();
	tOutline<cAddon>::draft();
	tOutline<cAddon_OSX>::draft();

	gWorld->flushLines();
#ifdef GTUT_GOOGLE
	::testing::InitGoogleTest(&argc, argv);
	result = RUN_ALL_TESTS();
#endif
	gWorld->flushLines();
	delete gt::gWorld;

	std::cout << "Tests over." << std::endl;

	return result;
}

#ifdef GTUT

	GTUT_START(addonOSX, load)
	{
		ptrFig window;
		ptrLead setPath = gWorld->makeLead(getHash<cAddon>(), cAddon::xLoadAddon->mID);
		cPlug<dStr> path = cPlug<dStr>("X11GL");

		addonGL = gWorld->makeFig(getHash<cAddon>());
		setPath->add(&path, cAddon::xPT_addonName);
		addonGL->jack(setPath);

		window = gWorld->makeFig(getHash<cWindowFrame>());
	}
	GTUT_END;

	GTUT_START(addonOSX, unload)
	{
		bool addonBlueprintRemoved = false;
		addonGL.reset();
		try{
			gWorld->getBlueprint(getHash<cWindowFrame>());
		}catch(excep::base_error){
			addonBlueprintRemoved = true;
		}
		GTUT_ASRT(addonBlueprintRemoved, "didn't remove blueprints.");
	}
	GTUT_END;

#endif
