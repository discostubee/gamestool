//!\file	main.cpp
//!\brief	Start of unit tests

#include "gt_terminal/entryPoint.hpp"

ENTRYPOINT
{
	int result = EXIT_FAILURE;

	std::cout << "Running gamestool tests.";
#	if defined(__APPLE__)
		std::cout << " For apple.";
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

	return result;
}

