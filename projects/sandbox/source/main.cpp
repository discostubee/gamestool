//!\file	main.cpp
//!\brief	Entry point for the gamestool.


#include "gt_base/anchor.hpp"
#include "gt_base/runList.hpp"

#ifdef __APPLE__
#	include "gt_OSX/entryPoint.hpp"
#	include "gt_OSX/OSX_fileIO.hpp"
#endif



void draftAll(){
	using namespace gt;

	//- This needs to be a complete list of everything in the gamestool lib.
	tOutline<cAnchor>::draft();
	tOutline<cRunList>::draft();
	tOutline<cBase_fileIO>::draft();

	//- Draft in stuff specific for platform.
#	if defined(__APPLE__)
		tOutline<cOSX_fileIO>::draft();
#	elif defined(__linux)
#	elif defined(WIN32)
#	endif
}

gt::ptrFig getRootAnchor(){
	using namespace gt;
	ptrFig root;
	ptrFig ank = gWorld.get()->makeFig(getHash<cAnchor>());
	ptrFig file = gWorld.get()->makeFig(getHash<cBase_fileIO>());
	return root;
}

ENTRYPOINT
{
	using namespace gt;

	gWorld.take(
#	if defined(__APPLE__)
		new cOSXWorld()
#	endif
	);

	draftAll();

	gWorld.get()->setRoot( getRootAnchor() );

	gWorld.get()->loop();

	return EXIT_SUCCESS;
}




