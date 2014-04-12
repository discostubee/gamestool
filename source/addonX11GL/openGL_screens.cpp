#include "openGL_screens.hpp"

using namespace gt;

cScreen_X11GL::cScreen_X11GL(){

}

cScreen_X11GL::~cScreen_X11GL(){

}

shape::tRectangle<dUnitPix> cScreen_X11GL::getDims(){
	return shape::tRectangle<dUnitPix>(0, 0, 100, 100);
}
