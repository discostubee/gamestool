/*
 * addonX11GL.cpp
 *
 *  Created on: 31/10/2010
 *      Author: stuandlou
 */

#include "addonPointyEars.hpp"
#include "alias.hpp"
#include "alucard.hpp"
#include "runList.hpp"
#include "valve.hpp"
#include "plugHound.hpp"

using namespace gt;

DYN_LIB_DEF(void)
draftAll(cWorld *pWorld){
	try{
		gt::cWorld::primordial::link(pWorld);
		gt::gWorld.take(pWorld);	//- Old world cleaned up by doing this.

		tOutline<cAlias>::draft();
		tOutline<cRunList>::draft();
		tOutline<cValve>::draft();
		tOutline<cPlugHound>::draft();

		DBUG_LO("Pointy Ear addon open.");

	}catch(excep::base_error &e){
		excep::delayExcep::add(e.what());

	}catch(...){
		excep::delayExcep::add("unknown error while opening Pointy Ears addon");
	}
}

DYN_LIB_DEF(void)
closeLib(){
	try{
		gt::cWorld::primordial::addonClosed(__FILE__);
	}catch(...){
	}
}
