/*
 * addonX11GL.cpp
 *
 *  Created on: 31/10/2010
 *      Author: stuandlou
 */

#include "addonMaths.hpp"


DYN_LIB_DEF(void)
draftAll(gt::cWorld *pWorld){
	try{
		gt::cWorld::primordial::link(pWorld);
		gt::gWorld.take(pWorld);	//- Old world cleaned up by doing this.


		DBUG_LO("Math addon open.");

	}catch(std::exception &e){
		excep::delayExcep::add(e.what());

	}catch(...){
		excep::delayExcep::add("unknown error while opening addon");
	}
}

DYN_LIB_DEF(void)
closeLib(){
	try{
		gt::cWorld::primordial::addonClosed(__FILE__);
	}catch(...){
	}
}
