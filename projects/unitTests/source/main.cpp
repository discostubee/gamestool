//!\file	main.cpp
//!\brief	Start of unit tests


// Include the stuff we want to test.
#include "gt_terminal/entryPoint.hpp"

ENTRYPOINT
{
	int result = EXIT_FAILURE;

	std::cout << "Running gamestool tests.";
#	ifdef __APPLE__
		std::cout << " For apple.";
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

	return result;
}

