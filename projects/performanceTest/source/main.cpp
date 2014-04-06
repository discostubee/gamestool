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
#elif defined(WIN32)
#endif

using namespace gt;


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
	gt::gWorld.cleanup();	//- Done here so that we can log destruction faults.
}



ENTRYPOINT
{
	try{
		gt::gWorld.take(new gt::cTerminalWorld());

		gWorld.get()->checkAddons();
		draftAll();



		excep::delayExcep::shake();
		cleanupAll();

	}catch(std::exception &e){
		try{ cleanupAll(); }catch(...){}
		std::cout << e.what();


	}catch(...){
		try{ cleanupAll(); }catch(...){}
		std::cout << "Unknown error.";
	}


	return EXIT_SUCCESS;
}


