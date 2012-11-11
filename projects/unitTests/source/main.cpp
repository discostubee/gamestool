//!\file	main.cpp
//!\brief	Start of unit tests


// Include the stuff we want to test.
#include "gt_base/figment.hpp"

int
main(int argc, char **argv){
	int result = EXIT_FAILURE;

	std::cout << "Running gamestool tests. Version 0.3";
	#ifdef GT_THREADS
		std::cout << " threaded";
	#endif
	std::cout << std::endl;

	gt::gWorld.take( new gt::cWorld() );

#ifdef GTUT_GOOGLE
	::testing::InitGoogleTest(&argc, argv);
	result = RUN_ALL_TESTS();
#endif

	gt::cWorld::primordial::makeProfileReport(std::cout);
	gt::gWorld.cleanup();
	cTracker::makeReport(std::cout);

	return result;
}

