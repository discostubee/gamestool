/*
 * addonX11GL.cpp
 *
 *  Created on: 31/10/2010
 *      Author: stuandlou
 */

#include "addonPNG.hpp"
#include "PNG_bitmap.hpp"

DYN_LIB_DEF(void)
draftAll(gt::cWorld *pWorld){
	try{
		gt::cWorld::primordial::redirectWorld(pWorld);
		gt::gWorld.take(pWorld);	//- Old world cleaned up by doing this.

		gt::tOutline<gt::cBitmap_PNG>::draft();


		DBUG_LO(gt::PNGAddon::getAddonName() << " addon open.");

	}catch(excep::base_error &e){
		excep::logExcep::add(e.what());

	}catch(...){
		excep::logExcep::add("unknown error while opening addon");
	}
}

DYN_LIB_DEF(void)
closeLib(){
	try{
		gt::tOutline<gt::cBitmap_PNG>::remove();

		DBUG_LO("closed " << gt::PNGAddon::getAddonName() << " addon.");

		gt::gWorld.drop();
		cTracker::makeReport(std::cout);

	}catch(excep::base_error &e){
		excep::logExcep::add(e.what());

	}catch(...){
		excep::logExcep::add("unknown error while closing addon");
	}
}
