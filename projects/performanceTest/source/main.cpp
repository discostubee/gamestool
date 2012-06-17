//!\file	main.cpp
//!\brief	


// Include the stuff we want to test.
#include "gt_base/figment.hpp"
#include "gt_OSX/OSX_world.hpp"
int
main(int argc, char **argv){
	int result = EXIT_FAILURE;

	std::cout << "Running performance tests. Version 0.1" << std::endl;

	gt::gWorld.take( new gt::cOSXWorld() );

	{
		using namespace gt;

	#ifdef GTUT_GOOGLE
		::testing::InitGoogleTest(&argc, argv);
		result = RUN_ALL_TESTS();
	#endif

		PROFILE;

		cContext fakeConx;
		for(int numTest=0; numTest<1000; ++numTest){
			ptrFig fig = gWorld.get()->makeFig(getHash<cFigment>());
			ptrLead lead = gWorld.get()->makeLead(cFigment::xTestJack, fakeConx.getSig());
			tPlug<int> life = 42;

			lead->addPlug(&life, cFigment::xPT_life);
			fig->jack(lead, &fakeConx);
			fig->run(&fakeConx);
		}
	}

	gt::gWorld.get()->makeProfileReport(std::cout);
	gt::gWorld.cleanup();
	cTracker::makeReport(std::cout);

	return result;
}

