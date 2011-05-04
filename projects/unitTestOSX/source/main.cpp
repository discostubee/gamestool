//!\file	main.cpp
//!\brief	Aims to test the world's ability to load addons, print lines and profile tokens.
//!\note	You need the  OSX GL addon.

// Include the stuff we want to test.
#include <gt_OSX/OSX_world.hpp>
#include <gt_OSX/OSX_addon.hpp>
#include <gt_graphics/windowFrame.hpp>

using namespace gt;

#ifdef GTUT

	GTUT_START(addons, load)
	{
		ptrFig aoGL= gWorld->makeFig(getHash<cAddon>());
		ptrFig window;
		ptrLead setPath = gWorld->makeLead(getHash<cAddon>(), cAddon::xLoadAddon->mID);
		cPlug<dStr> path = cPlug<dStr>("OSXGL");

		setPath->add(&path, cAddon::xPT_addonName);
		aoGL->jack(setPath);

		window = gWorld->makeFig(getHash<cWindowFrame>());
	}
	GTUT_END;


	GTUT_START(addons, sharedLines)
	{
	}
	GTUT_END;

	GTUT_START(addons, sharedProfile)
	{
	}
	GTUT_END;

#endif

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
