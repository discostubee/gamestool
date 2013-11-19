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

void draftAll(){
	using namespace gt;

	//- This needs to be a complete list of everything in the gamestool lib.
	tOutline<cFigment>::draft();
	tOutline<cEmptyFig>::draft();
	tOutline<cChainLink>::draft();
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

void cleanupAll(){

	tOutline<cEmptyFig>::remove();
	tOutline<cAnchor>::remove();
	tOutline<cRunList>::remove();
	tOutline<cBase_fileIO>::remove();
	tOutline<cAlias>::remove();
	tOutline<cFigFactory>::remove();
	tOutline<cWorldShutoff>::remove();
	tOutline<cTextFig>::remove();
	tOutline<cThread>::remove();
	tOutline<cValve>::remove();
	tOutline<cChainLink>::remove();
	tOutline<cFigment>::remove();
}

void openAddons(){
	gWorld.get()->openAddon(dStr("X11GL"));
}

ptrFig makeEditor(){

	cContext setupConx;
	ptrFig rlTop = gWorld.get()->makeFig(getHash<cRunList>());

	tPlug<ptrFig> stage = gWorld.get()->makeFig(getHash<cStage>());
	tPlug<ptrFig> film = gWorld.get()->makeFig(getHash<cFilm>());
	tPlug<ptrFig> cam = gWorld.get()->makeFig(getHash<c2DCamera>());
	tPlug<ptrFig> close = gWorld.get()->makeFig(getHash<cWorldShutoff>());
	tPlug<ptrFig> menu;

	{	//- link components.
		ptrLead linkFilm = gWorld.get()->makeLead(cStage::xSetLink);
		linkFilm->linkPlug(&film, cStage::xPT_links);
		stage.get()->jack(linkFilm, &setupConx);

		ptrLead linkClose = gWorld.get()->makeLead(cStage::xLinkCloser);
		linkClose->linkPlug(&close, cStage::xPT_closer);
		stage.get()->jack(linkClose, &setupConx);

		ptrLead linkCam = gWorld.get()->makeLead(cFilm::xSetLink);
		linkCam->linkPlug(&cam, cFilm::xPT_links);
		film.get()->jack(linkCam, &setupConx);

		tPlugLinearContainer<ptrFig, std::vector> contain;
		contain.add(stage);

		ptrLead topLinks = gWorld.get()->makeLead(cRunList::xAdd);
		topLinks->linkPlug(&contain, cRunList::xPT_link);
		rlTop->jack(topLinks, &setupConx);
	}

	return rlTop;
}

ENTRYPOINT
{
	try{
		gt::gWorld.take(new gt::cTerminalWorld());

		draftAll();
		openAddons();

		{
			tPlug<ptrBuff> buff(ptrBuff(new cByteBuffer()));
			cContext conxSetup;

			{
				tPlug<ptrFig> ank = gWorld.get()->makeFig(getHash<cAnchor>());
				tPlug<ptrFig> editor = makeEditor();

				ptrLead linkEdit = gWorld.get()->makeLead(cAnchor::xSetLink);
				linkEdit->linkPlug(&editor, cAnchor::xPT_links);
				ank.get()->jack(linkEdit, &conxSetup);

				ptrLead save = gWorld.get()->makeLead(cAnchor::xSave);
				save->linkPlug(&buff, cAnchor::xPT_serialBuff);
				ank.get()->jack(save, &conxSetup);
			}

			{	//- write file.
				tPlug<ptrFig> writer = gWorld.get()->makeFig(getHash<cBase_fileIO>());
				tPlug<dStr> path;
				ptrLead setPath = gWorld.get()->makeLead(cBase_fileIO::xSetPath);

				path = "editor.gtf";
				setPath->linkPlug(&path, cBase_fileIO::xPT_filePath);
				writer.get()->jack(setPath, &conxSetup);

				ptrLead write = gWorld.get()->makeLead(cBase_fileIO::xWrite);
				write->linkPlug(&buff, cBase_fileIO::xPT_buffer);
				writer.get()->jack(write, &conxSetup);
			}
		}

		cleanupAll();
		gt::gWorld.cleanup();	//- Done here so that we can log destruction faults.
		excep::logExcep::shake();

	}catch(std::exception &e){
		std::cout << e.what() << std::endl;
	}catch(...){
		std::cout << "Unknown error." << std::endl;
	}

	return EXIT_SUCCESS;
}


