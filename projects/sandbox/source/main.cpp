//!\file	main.cpp
//!\brief	Entry point for the gamestool.


#include "gt_base/alias.hpp"
#include "gt_base/anchor.hpp"
#include "gt_base/figment.hpp"
#include "gt_base/figFactory.hpp"
#include "gt_base/runList.hpp"
#include "gt_base/textFig.hpp"
#include "gt_base/thread.hpp"
#include "gt_base/valve.hpp"

#ifdef __APPLE__
#	include "gt_OSX/entryPoint.hpp"
#	include "gt_OSX/OSX_fileIO.hpp"
#elif defined(__linux)
#	include "gt_linux/entryPoint.hpp"
#	include "gt_linux/linux_fileIO.hpp"
#elif defined(WIN32)
#endif



void draftAll(){
	using namespace gt;

	//- This needs to be a complete list of everything in the gamestool lib.
	tOutline<cFigment>::draft();
	tOutline<cChainLink>::draft();
	tOutline<cRunList>::draft();
	tOutline<cValve>::draft();
	tOutline<cAlias>::draft();
	tOutline<cAnchor>::draft();
	tOutline<cEmptyFig>::draft();
	tOutline<cFigFactory>::draft();
	tOutline<cBase_fileIO>::draft();
	tOutline<cTextFig>::draft();
	tOutline<cThread>::draft();
	tOutline<cWorldShutoff>::draft();

	//- Draft in stuff specific for platform.
#	if defined(__APPLE__)
		tOutline<cOSX_fileIO>::draft();
#	elif defined(__linux)
		tOutline<cLinux_fileIO>::draft();
#	elif defined(WIN32)
#	endif
}

gt::ptrFig getRootAnchor(dNatStr & rootFile){
	using namespace gt;

	cContext conxLoading;
	tPlug<ptrFig> root;
	ptrFig ank = gWorld.get()->makeFig(getHash<cAnchor>());

	{
		tPlug<dStr> path(rootFile);
		tPlug<ptrBuff> buff(ptrBuff(new cByteBuffer()));
		tPlug<ptrFig> file = gWorld.get()->makeFig(getHash<cBase_fileIO>());

		ptrLead setPath = gWorld.get()->makeLead(cBase_fileIO::xSetPath);
		setPath->linkPlug(&path, cBase_fileIO::xPT_filePath);

		file.get()->jack(setPath, &conxLoading);

		ptrLead readFile = gWorld.get()->makeLead(cBase_fileIO::xRead);
		readFile->linkPlug(&buff, cBase_fileIO::xPT_buffer);
		file.get()->jack(readFile, &conxLoading);

		ptrLead load = gWorld.get()->makeLead(cAnchor::xLoad);
		load->linkPlug(&buff, cAnchor::xPT_serialBuff);
		ank->jack(load, &conxLoading);
	}

	{
		ptrLead getRoot = gWorld.get()->makeLead(cAnchor::xGetLinks);
		getRoot->linkPlug(&root, cAnchor::xPT_links);
		ank->jack(getRoot, &conxLoading);
	}

	return root.get();
}


ENTRYPOINT
{
	using namespace gt;
	int result = EXIT_FAILURE;

	try{
		std::stringstream ss;
		ss << "~Welcome " << getPlatformInfo() << "~";
		cWorld::primordial::lo(ss.str());

		gWorld.take(
#			if defined(__APPLE__)
				new cOSXWorld()
#			elif defined(__linux)
				new cLinuxWorld()
#			endif
		);

		try{
			dNatStr rootFile = toNStr("editor.gtf");
			gWorld.get()->checkAddons();
			draftAll();
			gWorld.get()->nameProgAndMakeFride(makeHash(rootFile));
			gWorld.get()->setRoot( getRootAnchor(rootFile) );
			gWorld.get()->loop();

		}catch(std::exception &e){
			excep::delayExcep::add(e.what());
		}
		catch(...){
			excep::delayExcep::add("Boom! Didn't see that coming.");
		}

		gWorld.cleanup();	//- Done here so that we can log destruction faults.
		excep::delayExcep::shake();
		result = EXIT_SUCCESS;

	}catch(std::exception &e){

	}catch(...){

	}

	return result;
}




