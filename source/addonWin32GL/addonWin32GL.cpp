#include "addonWin32GL.hpp"
#include "openGL_windowFrame.hpp"
#include "openGL_layer.hpp"

DYN_LIB_DEF(void)
draftAll(gt::cWorld* pWorld){
	try{
		gt::redirectWorld(pWorld);

		gt::tOutline<gt::cWindowFrame>::draft();
		gt::tOutline<gt::cWindowFrame_winGL>::draft();
		gt::tOutline<gt::cLayer>::draft();	
		gt::tOutline<gt::cLayer_winGL>::draft();

		DBUG_LO("Win32GL addon open.");
	}catch(excep::base_error &e){
		WARN(e);

	}catch(...){
		std::cout << "unknown error while opening x11 addon" << std::endl;
	}
}

DYN_LIB_DEF(void)
closeLib(){
	try{
		gt::tOutline<gt::cWindowFrame_winGL>::removeFromWorld();
		gt::tOutline<gt::cWindowFrame>::removeFromWorld();
		gt::tOutline<gt::cLayer_winGL>::removeFromWorld();
		gt::tOutline<gt::cLayer>::removeFromWorld();

		cTracker::makeReport(std::cout);

		DBUG_LO("Win32GL logic addon.");
	}catch(excep::base_error &e){
		WARN(e);

	}catch(...){
		std::cout << "unknown error while opening x11 addon" << std::endl;
	}
}
