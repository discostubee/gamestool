#include "openGL_bitmap.hpp"

using namespace gt;

cBitmap_X11GL::cBitmap_X11GL(){
}

cBitmap_X11GL::~cBitmap_X11GL(){
}

void
cBitmap_X11GL::run(cContext *pCon){
	if(!mPendingAdds.empty()){
		//!\todo

		mPendingAdds.pop_back();
	}
}

cBitmap::ptrBitmap
cBitmap_X11GL::getBitmap(){
	ptrBitmap rtnBM(new dChannels);

	//!\todo

	return rtnBM;
}
