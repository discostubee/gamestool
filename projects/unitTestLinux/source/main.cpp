//!\file	main.cpp
//!\brief	Aims to test the world's ability to load addons, print lines and profile tokens.
//!\note	You need the  OSX GL addon.

// Include the stuff we want to test.

#include "gt_linux/linux_world.hpp"
#include "gt_base/runList.hpp"
#include "gt_graphics/windowFrame.hpp"

#include <fstream>

#ifndef GTUT
#	error Specify gamestool testing, idiot.
#endif

using namespace gt;

int
main(int argc, char **argv){
	int result = EXIT_SUCCESS;
	std::cout
		<< "Running gamestool tests for linux. Version 0.2. "
	#if defined (GT_THREADS)
		<< "Threads."
	#endif
		<< std::endl;

	gWorld.take( new cLinuxWorld() );

	gWorld.get()->flushLines();
#ifdef GTUT_GOOGLE
	::testing::InitGoogleTest(&argc, argv);
	result = RUN_ALL_TESTS();
#endif

	std::ofstream profileReport("profile_report.txt");
	gt::gWorld.get()->makeProfileReport(profileReport);//(std::cout);
	profileReport.close();


	gt::gWorld.cleanup();

	return result;
}

GTUT_START(testLinux, load){
	gWorld.get()->openAddon("X11GL");
}GTUT_END;

GTUT_START(testLinux, unload){

}GTUT_END;
