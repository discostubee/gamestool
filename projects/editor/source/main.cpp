//!\file	main.cpp
//!\brief	This is the end point of the editor, so all the info about this project goes here.
//!			The editor should be the only program where all it does is build a gamestool app
//!			and then saves it. Another way to look at this project, is that it's not the
//!			actual editor, but rather a program that builds the editor.

#include "gt_base/anchor.hpp"
#include "gt_base/figFactory.hpp"
#include "gt_base/textFig.hpp"
#include "gt_base/thread.hpp"
#include "gt_base/chainLink.hpp"

#include "addonPointyEars/runList.hpp"
#include "addonPointyEars/alias.hpp"
#include "addonPointyEars/valve.hpp"

#include "gt_terminal/entryPoint.hpp"

//- Not done as part of the terminal entry.
#if defined(__APPLE__)
#	include "gt_OSX/OSX_fileIO.hpp"
#elif defined(__linux)
#	include "gt_linux/linux_fileIO.hpp"
#elif defined(WIN32)
#endif

#include "gt_graphics/screens.hpp"
#include "gt_graphics/stage.hpp"
#include "gt_graphics/film.hpp"
#include "gt_graphics/camera.hpp"
#include "gt_graphics/polygonMesh.hpp"
#include "gt_graphics/transform.hpp"

using namespace gt;

ptrFig makeEditor();

void draftAll(){
	using namespace gt;

	tOutline<cFigment>::draft();
	tOutline<cEmptyFig>::draft();
	tOutline<cChainLink>::draft();
	tOutline<cAnchor>::draft();
	tOutline<cBase_fileIO>::draft();
	tOutline<cFigFactory>::draft();
	tOutline<cUnicron>::draft();
	tOutline<cTextFig>::draft();
	tOutline<cThread>::draft();

#	if defined(__APPLE__)
		tOutline<cOSX_fileIO>::draft();
#	elif defined(__linux)
		tOutline<cLinux_fileIO>::draft();
#	elif defined(WIN32)
#	endif
}

ptrFig makeMesh(){
	dRefWorld world = gWorld.get();
	cContext setupConx;
	ptrFig rtn = world->makeFig(getHash<cTransform>());

	tPlug<ptrFig> list = world->makeFig("run list");
	ptrLead linkList = world->makeLead(cTransform::xSetLink);
	linkList->linkPlug(&list, cTransform::xPT_links);
	rtn->jack(linkList, &setupConx);

	ptrLead addStuff = world->makeLead("run list", "add");
	tPlug<ptrFig> mesh = world->makeFig(getHash<cPolyMesh>());
	addStuff->linkPlug(&mesh, cRunList::xPT_links);
	list.get()->jack(addStuff, &setupConx);

	ptrLead setMesh = world->makeLead(cPolyMesh::xAddToMesh);

	cPolyMesh::dPlugVerts verts;
	verts += sVertex(0.5,  0.0, 0);
	verts += sVertex(-0.5, 0.0, 0);
	verts += sVertex(0,    0.5, 0);
	setMesh->linkPlug(&verts, cPolyMesh::xPT_vertexs);

	cPolyMesh::dPlugPoly polys;
	polys += sPoly(0, 1, 2);
	setMesh->linkPlug(&polys, cPolyMesh::xPT_polies);

	mesh.get()->jack(setMesh, &setupConx);

	return rtn;
}

ptrFig makeEditor(){
	dRefWorld world = gWorld.get();
	cContext setupConx;
	ptrFig rlTop = world->makeFig("run list");

	tPlug<ptrFig> stage = world->makeFig(getHash<cStage>());
	tPlug<ptrFig> film = world->makeFig(getHash<cFilm>());
	tPlug<ptrFig> mesh = makeMesh();
	tPlug<ptrFig> cam = world->makeFig(getHash<cCamera2D>());
	tPlug<ptrFig> close = world->makeFig(getHash<cUnicron>());

	{	//- Setup stage
		tPlug<ptrFig> screen = world->makeFig(getHash<cScreen>());
		tPlug< shape::tRectangle<dUnitPix> > shapeScreen;
		ptrLead getScreen = world->makeLead(cScreen::xGetShape);
		getScreen->linkPlug(&shapeScreen, cScreen::xPT_rect);
		screen.get()->jack(getScreen, &setupConx);

		shapeScreen.get().bottom *= 0.5;
		shapeScreen.get().top *= 0.5;
		shapeScreen.get().left *= 0.5;
		shapeScreen.get().right *= 0.5;

		ptrLead setLayout  = world->makeLead(cStage::xGetLayout);
		setLayout->linkPlug(&shapeScreen, cStage::xPT_layout);
		stage.get()->jack(setLayout, &setupConx);
	}

	{	//- link components.
		ptrLead linkMesh = world->makeLead(cChainLink::xSetLink);
		linkMesh->linkPlug(&mesh, cChainLink::xPT_links);
		cam.get()->jack(linkMesh, &setupConx);

		ptrLead linkFilm = world->makeLead(cChainLink::xSetLink);
		linkFilm->linkPlug(&film, cChainLink::xPT_links);
		stage.get()->jack(linkFilm, &setupConx);

		ptrLead linkCam = world->makeLead(cChainLink::xSetLink);
		linkCam->linkPlug(&cam, cChainLink::xPT_links);
		film.get()->jack(linkCam, &setupConx);

		ptrLead linkClose = world->makeLead(cStage::xLinkCloser);
		linkClose->linkPlug(&close, cStage::xPT_links);
		stage.get()->jack(linkClose, &setupConx);

		tPlugLinearContainer<ptrFig, std::vector> contain;
		contain.add(stage);

		ptrLead topLinks = world->makeLead("run list", "add");
		topLinks->linkPlug(&contain, cRunList::xPT_links);
		rlTop->jack(topLinks, &setupConx);
	}

	return rlTop;
}

ENTRYPOINT
{
	int rtn = EXIT_FAILURE;

	try{
		cWorld::primordial::lo("Making editor file.");
		gt::gWorld.take(new gt::cTerminalWorld());

		gWorld.get()->checkAddons();
		draftAll();
		gWorld.get()->nameProgAndMakeFridge(makeHash("editor"));

		try{
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
		}catch(std::exception &e){
			excep::delayExcep::add(e.what());
		}
		catch(...){
			excep::delayExcep::add("Boom! Didn't see that coming.");
		}

		gt::gWorld.cleanup();	//- Because we can't be certain of the order globals are destroyed, you need to call this explicitly or risk loosing logs.
		cWorld::primordial::lo("Ended.");
		excep::delayExcep::shake();

		rtn = EXIT_SUCCESS;

	}catch(std::exception &e){
		std::cout << e.what() << std::endl;

	}catch(...){
		std::cout << "Unknown error." << std::endl;
	}

	return rtn;
}


