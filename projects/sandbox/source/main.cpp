//!\file	main.cpp
//!\brief	Entry point for the gamestool.


#include "gt_base/anchor.hpp"
#include "gt_base/runList.hpp"
#include "gt_base/alias.hpp"
#include "gt_base/figFactory.hpp"
#include "gt_base/textFig.hpp"
#include "gt_base/thread.hpp"
#include "gt_base/valve.hpp"

#ifdef __APPLE__
#	include "gt_OSX/entryPoint.hpp"

#	elif defined(__linux)
#	elif defined(WIN32)
#endif



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

gt::ptrFig getRootAnchor(){
	using namespace gt;

	cContext conxLoading;

	ptrFig file = gWorld.get()->makeFig(getHash<cBase_fileIO>());
	{
		tPlug<dStr> path;
		ptrLead setPath = gWorld.get()->makeLead(cBase_fileIO::xSetPath);

		//todo allow sandbox to load from another file.
		path = "editor.gtf";
		setPath->setPlug(&path, cBase_fileIO::xPT_filePath);
		file->jack(setPath, &conxLoading);
	}

	ptrFig ank = gWorld.get()->makeFig(getHash<cAnchor>());
	{
		tPlug<ptrFig> plugFile;
		ptrLead load = gWorld.get()->makeLead(cAnchor::xLoad);

		plugFile = file;
		load->setPlug(&plugFile, cAnchor::xPT_serialBuff);
		ank->jack(load, &conxLoading);
	}

	ptrFig root;
	{
		tPlug<ptrFig> plugRoot;
		ptrLead getRoot = gWorld.get()->makeLead(cAnchor::xGetRoot);

		getRoot->setPlug(&plugRoot, cAnchor::xPT_root);
		ank->jack(getRoot, &conxLoading);

		root = plugRoot.get();
	}

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




