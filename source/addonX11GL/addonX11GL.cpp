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

		gt::tOutline<gt::cFigment>::draft();
		gt::tOutline<gt::cEmptyFig>::draft();

		gt::tOutline<gt::cWindowFrame>::draft();
		gt::tOutline<gt::cWindowFrame_X11GL>::draft();
		gt::tOutline<gt::cLayer>::draft();
		gt::tOutline<gt::cLayer_X11GL>::draft();
		gt::tOutline<gt::cPolyMesh>::draft();
		gt::tOutline<gt::cPolyMesh_X11GL>::draft();
		gt::tOutline<gt::c3DCamera>::draft();
		gt::tOutline<gt::c3DCamera_X11GL>::draft();
		gt::tOutline<gt::c2DCamera>::draft();
		gt::tOutline<gt::c2DCamera_X11GL>::draft();

		DBUG_LO("x11 addon open.");

	}catch(excep::base_error &e){
		WARN(e.what());

	}catch(...){
		WARN_S("error while opening x11 addon");
	}
}

DYN_LIB_DEF(void)
closeLib(){
	try{
		gt::tOutline<gt::cWindowFrame_X11GL>::removeFromWorld();
		gt::tOutline<gt::cWindowFrame>::removeFromWorld();
		gt::tOutline<gt::cLayer_X11GL>::removeFromWorld();
		gt::tOutline<gt::cLayer>::removeFromWorld();
		gt::tOutline<gt::cPolyMesh_X11GL>::removeFromWorld();
		gt::tOutline<gt::cPolyMesh>::removeFromWorld();
		gt::tOutline<gt::c3DCamera_X11GL>::removeFromWorld();
		gt::tOutline<gt::c3DCamera>::removeFromWorld();
		gt::tOutline<gt::c2DCamera_X11GL>::removeFromWorld();
		gt::tOutline<gt::c2DCamera>::removeFromWorld();

		DBUG_LO("closed x11 addon.");

		gt::cWorld::primordial::redirectWorld(NULL);

	}catch(excep::base_error &e){
		WARN(e);

	}catch(...){
		std::cout << "unknown error while closing x11 addon" << std::endl;
	}
}
