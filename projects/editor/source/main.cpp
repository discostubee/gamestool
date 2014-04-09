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
#include "gt_base/chainLink.hpp"

#include "gt_terminal/entryPoint.hpp"

//- Not done as part of the terminal entry.
#if defined(__APPLE__)
#	include "gt_OSX/OSX_fileIO.hpp"
#elif defined(__linux)
#	include "gt_linux/linux_fileIO.hpp"
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
		tOutline<cLinux_fileIO>::draft();
#	elif defined(WIN32)
#	endif
}

ptrFig makeEditor(){

	cContext setupConx;
	ptrFig rlTop = gWorld.get()->makeFig(getHash<cRunList>());

	tPlug<ptrFig> stage = gWorld.get()->makeFig("stage");
	tPlug<ptrFig> film = gWorld.get()->makeFig("film");
	tPlug<ptrFig> cam = gWorld.get()->makeFig("camera 2d");
	tPlug<ptrFig> menu = gWorld.get()->makeFig("poly mesh");
	tPlug<ptrFig> close = gWorld.get()->makeFig(getHash<cWorldShutoff>());

	{	//- link components.
		ptrLead linkFilm = gWorld.get()->makeLead("chain link", "set link");
		linkFilm->linkPlug(
			&film,
			gWorld.get()->getPlugTag("chain link", "links")
		);
		stage.get()->jack(linkFilm, &setupConx);

		ptrLead linkClose = gWorld.get()->makeLead("stage", "link closer");
		linkClose->linkPlug(
			&close,
			gWorld.get()->getPlugTag("chain link", "links")
		);
		stage.get()->jack(linkClose, &setupConx);

		ptrLead linkCam = gWorld.get()->makeLead("chain link", "set link");
		linkCam->linkPlug(
			&cam,
			gWorld.get()->getPlugTag("chain link", "links")
		);
		film.get()->jack(linkCam, &setupConx);

		tPlugLinearContainer<ptrFig, std::vector> contain;
		contain.add(stage);

		ptrLead topLinks = gWorld.get()->makeLead(cRunList::xAdd);
		topLinks->linkPlug(&contain, cRunList::xPT_links);
		rlTop->jack(topLinks, &setupConx);
	}

	return rlTop;
}

ENTRYPOINT
{
	try{
		cWorld::primordial::lo("Making editor file.");
		gt::gWorld.take(new gt::cTerminalWorld());

		gWorld.get()->checkAddons();
		draftAll();

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

		gt::gWorld.cleanup();	//- Done here so that we can log destruction faults.
		cWorld::primordial::lo("Ended.");
		excep::delayExcep::shake();


	}catch(std::exception &e){
		std::cout << e.what() << std::endl;


	}catch(...){
		std::cout << "Unknown error." << std::endl;
	}

	return EXIT_SUCCESS;
}


