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
		gt::redirectWorld(pWorld);

		gt::tOutline<gt::cWindowFrame>::draft();
		gt::tOutline<gt::cWindowFrame_X11GL>::draft();
		gt::tOutline<gt::cLayer>::draft();
		gt::tOutline<gt::cLayer_X11GL>::draft();
		gt::tOutline<gt::cPolyMesh>::draft();
		gt::tOutline<gt::cPolyMesh_GL>::draft();

		DBUG_LO("x11 addon open.");

	}catch(excep::base_error &e){
		WARN(e);

	}catch(...){
		std::cout << "unknown error while opening x11 addon" << std::endl;
	}
}

DYN_LIB_DEF(void)
closeLib(){
	try{
		gt::tOutline<gt::cWindowFrame_X11GL>::removeFromWorld();
		gt::tOutline<gt::cWindowFrame>::removeFromWorld();
		gt::tOutline<gt::cLayer_X11GL>::removeFromWorld();
		gt::tOutline<gt::cLayer>::removeFromWorld();
		gt::tOutline<gt::cPolyMesh>::removeFromWorld();
		gt::tOutline<gt::cPolyMesh_GL>::removeFromWorld();

		DBUG_LO("closed x11 addon.");

		gt::redirectWorld(NULL);

	}catch(excep::base_error &e){
		WARN(e);

	}catch(...){
		std::cout << "unknown error while closing x11 addon" << std::endl;
	}
}
