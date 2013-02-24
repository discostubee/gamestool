//!\file	main.cpp
//!\brief	


// Include the stuff we want to test.
#include "gt_base/figment.hpp"

#if defined(__APPLE__)
#	include "gt_OSX/OSX_world.hpp"
#elif defined(__linux__)
#	include "gt_linux/linux_world.hpp"
#endif

int
main(int argc, char **argv){
	int result = EXIT_FAILURE;

	std::cout << "Running performance tests. Version 0.1" << std::endl;

	#if defined(__APPLE__)
		gt::gWorld.take( new gt::cOSXWorld() );
	#elif defined(__linux__)
		gt::gWorld.take( new gt::cLinuxWorld() );
	#endif

	try{
		using namespace gt;

	#ifdef GTUT_GOOGLE
		::testing::InitGoogleTest(&argc, argv);
		result = RUN_ALL_TESTS();
	#endif

		PROFILE;

		cContext fakeConx;
		for(int numTest=0; numTest<1000; ++numTest){
			ptrFig fig = gWorld.get()->makeFig(getHash<cFigment>());
			ptrLead lead = gWorld.get()->makeLead(cFigment::xSave);
			tPlug<int> life = 42;

			fig->jack(lead, &fakeConx);
			fig->run(&fakeConx);
		}

		gt::gWorld.get()->makeProfileReport(std::cout);
		gt::gWorld.cleanup();
	}catch(std::exception &e){
		std::cout << "!Stuffed: " << e.what() << std::endl;
	}

	cTracker::makeReport(std::cout);

	return result;
}

