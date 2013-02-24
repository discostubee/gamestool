//!\file	main.cpp
//!\brief	Start of unit tests

#include "gt_terminal/entryPoint.hpp"

//- Include anything else we want to unit test here.
#include "gt_graphics/windowFrame.hpp"

ENTRYPOINT
{
	int result = EXIT_FAILURE;

	try{
		std::cout << "Running gamestool tests.";
#	if defined(__APPLE__)
		std::cout << " For Apple.";
#	elif defined(__linux)
		std::cout << " For Linux";
#	elif defined(_WIN32)
		std::cout << " For Windows.";
#	endif

#	ifdef GT_THREADS
		std::cout << " Threaded";
#	endif
		std::cout << std::endl;

		gt::gWorld.take( new gt::cTerminalWorld() );

#	ifdef GTUT_GOOGLE
		::testing::InitGoogleTest(&argc, argv);
		result = RUN_ALL_TESTS();
#	endif

		gt::cWorld::primordial::makeProfileReport(std::cout);
		gt::gWorld.cleanup();
		cTracker::makeReport(std::cout);
		excep::logExcep::shake();

	}catch(std::exception &e){
		std::cout << e.what() << std::endl;

	}catch(...){
		std::cout << "Unknown error." << std::endl;
	}
	return result;
}



#ifdef GTUT
namespace gt{

GTUT_START(test_addon, load){
	cContext fake;

	gWorld.get()->openAddon(dStr("X11GL"));

	gWorld.get()->setRoot(
		gWorld.get()->makeFig( getHash<cWindowFrame>() )
	);
}GTUT_END;

GTUT_START(test_addon, unload){
	tOutline<cWorldShutoff>::draft();

	bool addonBlueprintRemoved = false;

	gWorld.get()->setRoot(
		gWorld.get()->makeFig( getHash<cWorldShutoff>() )
	);	// Will take out everything, and should close the addon.

	gWorld.get()->loop();

	try{
		gWorld.get()->getBlueprint(getHash<cWindowFrame>());
	}catch(excep::base_error){
		DBUG_LO("removed the window frame blueprint.");
		addonBlueprintRemoved = true;
	}
	GTUT_ASRT(addonBlueprintRemoved, "didn't remove blueprints.");
}GTUT_END;

}
#endif
