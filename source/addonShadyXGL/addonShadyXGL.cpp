/*
 * addonX11GL.cpp
 *
 *  Created on: 31/10/2010
 *      Author: stuandlou
 */

#include "addonX11GL.hpp"

#include "openGL_stage.hpp"
#include "openGL_film.hpp"
#include "openGL_polyMesh.hpp"
#include "openGL_camera.hpp"

DYN_LIB_DEF(void)
draftAll(gt::cWorld *pWorld){
	try{
		gt::cWorld::primordial::redirectWorld(pWorld);
		gt::gWorld.take(pWorld);	//- Old world cleaned up by doing this.

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

	}catch(std::exception &e){
		excep::delayExcep::add(e.what());

	}catch(...){
		excep::delayExcep::add("unknown error while opening addon");
	}
}

DYN_LIB_DEF(void)
closeLib(){
	try{
		gt::tOutline<gt::cFilm_X11GL>::remove();
		gt::tOutline<gt::cFilm>::remove();
		gt::tOutline<gt::cPolyMesh_X11GL>::remove();
		gt::tOutline<gt::cPolyMesh>::remove();
		gt::tOutline<gt::c3DCamera_X11GL>::remove();
		gt::tOutline<gt::c3DCamera>::remove();
		gt::tOutline<gt::c2DCamera_X11GL>::remove();
		gt::tOutline<gt::c2DCamera>::remove();
		gt::tOutline<gt::cStage_X11GL>::remove();
		gt::tOutline<gt::cStage>::remove();

		DBUG_LO("closed x11 addon.");

		gt::gWorld.drop();
		cTracker::makeReport(std::cout);

	}catch(std::exception &e){
		excep::delayExcep::add(e.what());

	}catch(...){
		excep::delayExcep::add("unknown error while closing addon");
	}
}
