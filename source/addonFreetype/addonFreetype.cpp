/*
 * addonX11GL.cpp
 *
 *  Created on: 31/10/2010
 *      Author: stuandlou
 */

#include "addonFreetype.hpp"
#include "printFreetype.hpp"

DYN_LIB_DEF(void)
draftAll(gt::cWorld *pWorld){
	try{
		gt::cWorld::primordial::redirectWorld(pWorld);
		gt::gWorld.take(pWorld);	//- Old world cleaned up by doing this.

		gt::tOutline<gt::cPrintworks_freetype>::draft();

		DBUG_LO("Freetype addon open.");

	}catch(excep::base_error &e){
		excep::delayExcep::add(e.what());

	}catch(...){
		excep::delayExcep::add("unknown error while opening Freetype addon");
	}
}

DYN_LIB_DEF(void)
closeLib(){
	try{
		gt::tOutline<gt::cPrintworks_freetype>::remove();

		DBUG_LO("closed Freetype addon.");

		gt::gWorld.drop();
		cTracker::makeReport(std::cout);

	}catch(excep::base_error &e){
		excep::delayExcep::add(e.what());

	}catch(...){
		excep::delayExcep::add("unknown error while closing Freetype addon");
	}
}
