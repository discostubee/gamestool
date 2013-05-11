//!\file	main.cpp
//!\brief	This is the end point of the editor, so all the info about this project goes here.
//!			The editor should be the only program where all it does is build a gamestool app
//!			and then saves it. Another way to look at this project, is that it's not the
//!			actual editor, but rather a program that builds the editor.


#include "gt_terminal/entryPoint.hpp"
#include "gt_base/anchor.hpp"
#include "gt_base/runList.hpp"
#include "gt_graphics/stage.hpp"
#include "gt_graphics/film.hpp"
#include "gt_graphics/camera.hpp"

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


ptrFig makeEditor(){
#if defined(__APPLE__)
	gWorld.get()->openAddon("X11GL");
#elif defined(__linux)
	gWorld.get()->openAddon("X11GL");
#elif defined(_win32)
#endif

	ptrFig save = gWorld.get()->makeFig(getHash<cAnchor>());
	ptrFig rl = gWorld.get()->makeFig(getHash<cRunList>());
	ptrFig stage = gWorld.get()->makeFig(getHash<cStage>());
	ptrFig film = gWorld.get()->makeFig(getHash<cFilm>());
	ptrFig cam = gWorld.get()->makeFig(getHash<c2DCamera>());

	return rl;
}

