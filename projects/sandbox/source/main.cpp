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
	tOutline<cWorldShutoff>::draft();
	tOutline<cChainLink>::draft();
	tOutline<cAnchor>::draft();
	tOutline<cRunList>::draft();
	tOutline<cBase_fileIO>::draft();
	tOutline<cAlias>::draft();
	tOutline<cFigFactory>::draft();
	tOutline<cTextFig>::draft();
	tOutline<cThread>::draft();
	tOutline<cValve>::draft();
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

	cContext conxLoading;
	tPlug<ptrFig> root;
	tPlug<ptrFig> file = gWorld.get()->makeFig(getHash<cBase_fileIO>());
	ptrFig ank = gWorld.get()->makeFig(getHash<cAnchor>());

	{
		tPlug<dStr> path;
		ptrLead setPath = gWorld.get()->makeLead(cBase_fileIO::xSetPath);

		//todo allow sandbox to load from another file.
		path = "editor.gtf";
		setPath->setPlug(&path, cBase_fileIO::xPT_filePath);
		file.get()->jack(setPath, &conxLoading);

		ptrLead load = gWorld.get()->makeLead(cAnchor::xLoad);
		load->setPlug(&file, cAnchor::xPT_serialBuff);
		ank->jack(load, &conxLoading);
	}

	{
		ptrLead getRoot = gWorld.get()->makeLead(cAnchor::xGetLinks);
		getRoot->setPlug(&root, cAnchor::xPT_links);
		ank->jack(getRoot, &conxLoading);
	}

	return root.get();
}

ENTRYPOINT
{
	using namespace gt;
	try{
		gWorld.take(
#			if defined(__APPLE__)
				new cOSXWorld()
#			endif
		);

		try{
			draftAll();

			gWorld.get()->setRoot( getRootAnchor() );
			gWorld.get()->loop();
		}catch(std::exception &e){
			excep::logExcep::add(e.what());
		}
		catch(...){
			excep::logExcep::add("Boom! Didn't see that coming.");
		}

		gWorld.cleanup();	//- Done here so that we can log destruction faults.
		excep::logExcep::shake();

	}catch(std::exception &e){
		//todo trigger stack dump
		return EXIT_FAILURE;
	}catch(...){
		//todo trigger stack dump
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}




