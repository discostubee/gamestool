//!\file	main.cpp
//!\brief	This is the end point of the editor, so all the info about this project goes here.
//!			The editor should be the only program where all it does is build a gamestool app
//!			and then saves it. Another way to look at this project, is that it's not the
//!			actual editor, but rather a program that builds the editor.


#include "gt_base/anchor.hpp"
#include "gt_base/runList.hpp"
#include "gt_base/alias.hpp"
#include "gt_base/figFactory.hpp"
#include "gt_base/textFig.hpp"
#include "gt_base/thread.hpp"
#include "gt_base/valve.hpp"

#include "gt_graphics/stage.hpp"
#include "gt_graphics/film.hpp"
#include "gt_graphics/camera.hpp"

#include "gt_terminal/entryPoint.hpp"

//- Not done as part of the terminal entry.
#if defined(__APPLE__)
#	include "gt_OSX/OSX_fileIO.hpp"
#elif defined(__linux)
#elif defined(WIN32)
#endif

using namespace gt;

ptrFig makeEditor();

ENTRYPOINT
{

	gt::gWorld.take(new gt::cTerminalWorld());

	tPlug<ptrFig> save = gWorld.get()->makeFig(getHash<cAnchor>());
	ptrLead linkEdit = gWorld.get()->makeLead(cAnchor::)
	 makeEditor();

	return EXIT_SUCCESS;
}

void draftAll(){
	using namespace gt;

	//- This needs to be a complete list of everything in the gamestool lib.
	tOutline<cFigment>::draft();
	tOutline<cEmptyFig>::draft();
	tOutline<cAnchor>::draft();
	tOutline<cRunList>::draft();
	tOutline<cBase_fileIO>::draft();
	tOutline<cAlias>::draft();
	tOutline<cFigFactory>::draft();
	tOutline<cWorldShutoff>::draft();
	tOutline<cTextFig>::draft();
	tOutline<cThread>::draft();
	tOutline<cValve>::draft();

	//- Draft in stuff specific for platform.
#	if defined(__APPLE__)
		tOutline<cOSX_fileIO>::draft();
#	elif defined(__linux)
#	elif defined(WIN32)
#	endif
}

void openAddons(){
	gWorld.get()->openAddon(dStr("X11GL"));
}

ptrFig makeEditor(){
	draftAll();

	openAddons();

	cContext setupConx;
	ptrFig rlTop = gWorld.get()->makeFig(getHash<cRunList>());

	tPlug<ptrFig> stage = gWorld.get()->makeFig(getHash<cStage>());
	tPlug<ptrFig> film = gWorld.get()->makeFig(getHash<cFilm>());
	tPlug<ptrFig> cam = gWorld.get()->makeFig(getHash<c2DCamera>());
	tPlug<ptrFig> close = gWorld.get()->makeFig(getHash<cWorldShutoff>());
	tPlug<ptrFig> menu;

	{	//- link components.
		ptrLead linkFilm = gWorld.get()->makeLead(cStage::xSetLink);
		linkFilm->setPlug(&film, cStage::xPT_link);
		stage.get()->jack(linkFilm, &setupConx);

		ptrLead linkClose = gWorld.get()->makeLead(cStage::xLinkCloser);
		linkClose->setPlug(&close, cStage::xPT_closer);
		stage.get()->jack(linkClose, &setupConx);

		ptrLead linkCam = gWorld.get()->makeLead(cFilm::xSetLink);
		linkCam->setPlug(&cam, cFilm::xPT_link);
		film.get()->jack(linkCam, &setupConx);
	}

	{	//- write file.
		tPlug<ptrFig> writer = gWOrld.get()->makeFig(getHash<cBase_fileIO>())
		tPlug<dStr> path;
		ptrLead setPath = gWorld.get()->makeLead(cBase_fileIO::xSetPath);

		path = "editor.gtf";
		setPath->setPlug(&path, cBase_fileIO::xPT_filePath);
		writer->jack(setPath);

		ptrLead write = gWorld.get()->makeLead(cBase_fileIO::xWrite);
	}

	return rlTop;
}

