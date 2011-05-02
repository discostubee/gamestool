//!\file	main.cpp
//!\brief	

// Include the stuff we want to test.
#include <gt_base/utils.hpp>


int
main(int argc, char **argv){

	std::cout << "Running gamestool tests. Version 0.1" << std::endl;
#ifdef GTUT_GOOGLE
	::testing::InitGoogleTest(&argc, argv);
	(void)RUN_ALL_TESTS();

	std::cout << "Tests over. Press any key to finish" << std::endl;
	getchar();
#endif

	return EXIT_SUCCESS;
}