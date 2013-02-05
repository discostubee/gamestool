//!\file	main.cpp
//!\brief	Entry point for the gamestool.


#ifdef __APPLE__
#	include "gt_OSX/entryPoint.hpp"
#endif

ENTRYPOINT
{

	gt::gWorld.take(
#	ifdef __APPLE__
		new gt::cOSXWorld()
#	endif
	);
	gt::gWorld->loop();

	return EXIT_SUCCESS;
}

