//!\file	main.cpp
//!\brief	Entry point for the gamestool.


#ifdef __APPLE__
#	include "gt_OSX/entryPoint.hpp"
#endif

ptrFig getRootAnchor();

ENTRYPOINT
{

	gt::gWorld.take(
#	if defined(__APPLE__)
		new gt::cOSXWorld()
#	endif
	);

	gt::gWorld->setRoot( getRootAnchor() );

	gt::gWorld->loop();

	return EXIT_SUCCESS;
}

#if defined(__APPLE__)
ptrFig getRootAnchor(){

}
#elif defined(__linux)
ptrFig getRootAnchor(){

}
#elif defined(_win32)
ptrFig getRootAnchor(){

}
#endif
