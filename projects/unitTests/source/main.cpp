//!\file	main.cpp
//!\brief	Start of unit tests

#include "gt_terminal/entryPoint.hpp"
#include "gt_base/draftAll.hpp"

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
		gt::gWorld.get()->checkAddons();
		gt::draftAll();

#	ifdef GTUT_GOOGLE
		::testing::InitGoogleTest(&argc, argv);
		result = RUN_ALL_TESTS();
#	endif

		gt::gWorld.cleanup();
		gt::cWorld::primordial::cleanup();
		gt::cWorld::primordial::makeProfileReport(std::cout);
		cTracker::makeReport(std::cout);
		excep::logExcep::shake();
		result = EXIT_SUCCESS;

	}catch(std::exception &e){
		std::cout << e.what() << std::endl;

	}catch(...){
		std::cout << "Unknown error." << std::endl;
	}
	return result;
}


