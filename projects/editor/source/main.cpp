//!\file	main.cpp
//!\brief	This is the end point of the editor, so all the info about this project goes here.
//!			The editor should be the only program where all it does is build a gamestool app
//!			and then saves it. Another way to look at this project, is that it's not the
//!			actual editor, but rather a program that builds the editor.


#include "gt_terminal/entryPoint.hpp"

ptrFig makeEditor();

ENTRYPOINT
{

	gt::gWorld.take(
#	ifdef __APPLE__
		new gt::cOSXWorld()
#	endif
	);

	gt::gWorld->setRoot( makeEditor() );

	return EXIT_SUCCESS;
}

#if defined(__APPLE__)
ptrFig makeEditor(){

}
#elif defined(__linux)
ptrFig makeEditor(){

}
#elif defined(_win32)
ptrFig makeEditor(){

}
#endif
