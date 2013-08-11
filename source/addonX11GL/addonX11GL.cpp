/*
 * addonX11GL.cpp
 *
 *  Created on: 31/10/2010
 *      Author: stuandlou
 */

#include "addonX11GL.hpp"

DYN_LIB_DEF(void)
draftAll(gt::cWorld *pWorld){
	try{
		gt::cWorld::primordial::redirectWorld(pWorld);
		gt::gWorld.take(pWorld);	//- Old world cleaned up by doing this.

		gt::tOutline<gt::cFigment>::draft();
		gt::tOutline<gt::cEmptyFig>::draft();
		gt::tOutline<gt::cStage>::draft();
		gt::tOutline<gt::cStage_X11GL>::draft();
		gt::tOutline<gt::cFilm>::draft();
		gt::tOutline<gt::cFilm_X11GL>::draft();
		gt::tOutline<gt::cPolyMesh>::draft();
		gt::tOutline<gt::cPolyMesh_X11GL>::draft();
		gt::tOutline<gt::c3DCamera>::draft();
		gt::tOutline<gt::c3DCamera_X11GL>::draft();
		gt::tOutline<gt::c2DCamera>::draft();
		gt::tOutline<gt::c2DCamera_X11GL>::draft();

		DBUG_LO("x11 addon open.");

	}catch(excep::base_error &e){
		excep::logExcep::add(e.what());

	}catch(...){
		excep::logExcep::add("unknown error while opening X11GL addon");
	}
}

DYN_LIB_DEF(void)
closeLib(){
	try{

		gt::tOutline<gt::cStage_X11GL>::removeFromWorld();
		gt::tOutline<gt::cStage>::removeFromWorld();
		gt::tOutline<gt::cFilm_X11GL>::removeFromWorld();
		gt::tOutline<gt::cFilm>::removeFromWorld();
		gt::tOutline<gt::cPolyMesh_X11GL>::removeFromWorld();
		gt::tOutline<gt::cPolyMesh>::removeFromWorld();
		gt::tOutline<gt::c3DCamera_X11GL>::removeFromWorld();
		gt::tOutline<gt::c3DCamera>::removeFromWorld();
		gt::tOutline<gt::c2DCamera_X11GL>::removeFromWorld();
		gt::tOutline<gt::c2DCamera>::removeFromWorld();

		DBUG_LO("closed x11 addon.");

		gt::gWorld.drop();

	}catch(excep::base_error &e){
		excep::logExcep::add(e.what());

	}catch(...){
		excep::logExcep::add("unknown error while closing X11GL addon");
	}
}
