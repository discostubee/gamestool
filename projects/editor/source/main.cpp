//!\file	main.cpp
//!\brief	This is the end point of the editor, so all the info about this project goes here.
//!			The editor should be the only program where all it does is build a gamestool app
//!			and then saves it. Another way to look at this project, is that it's not the
//!			actual editor, but rather a program that builds the editor.


#include "gt_terminal/entryPoint.hpp"

#include "gt_base/runList.hpp"

using namespace gt;

ptrFig makeEditor();

ENTRYPOINT
{

	gt::gWorld.take(
#	if defined(__APPLE__)
		new gt::cOSXWorld()
#	elif defined(__linux)
		new gt::cLinuxWorld()
#	endif
	);

	gt::gWorld.get()->setRoot( makeEditor() );

	return EXIT_SUCCESS;
}

#if defined(__APPLE__)
ptrFig makeEditor(){
	gWorld.get()->openAddon("X11GL");
	ptrFig rl = gWorld.get()->makeFig("run list");

	return rl;
}
#elif defined(__linux)
ptrFig makeEditor(){
	gWorld.get()->openAddon("X11GL");
}
#elif defined(_win32)
ptrFig makeEditor(){

}
#endif
