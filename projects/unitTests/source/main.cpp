//!\file	main.cpp
//!\brief	

// Include the stuff we want to test.
#include <gt_base/figment.hpp>


int
main(int argc, char **argv){

	std::cout << "Running gamestool tests. Version 0.1" << std::endl;

	gt::gWorld = new gt::cWorld();
#ifdef GTUT_GOOGLE
	::testing::InitGoogleTest(&argc, argv);
	RUN_ALL_TESTS();
#endif
	delete gt::gWorld;

	std::cout << "Tests over. Press any key to finish" << std::endl;
	getchar();

	return EXIT_SUCCESS;
}
