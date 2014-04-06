#include "openGL_bitmap.hpp"

using namespace gt;

cBitmap_X11GL::cBitmap_X11GL(){
}

cBitmap_X11GL::~cBitmap_X11GL(){
}

cBitmap::ptrBitmap
cBitmap_X11GL::getBitmap(){
	return ptrBitmap(NULL);
}

void
cBitmap_X11GL::applyDirt(const dListDirty pDirt){

}
