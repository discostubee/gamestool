#include "PNG_bitmap.hpp"

using namespace gt;

cBitmap_PNG::cBitmap_PNG(){
}

cBitmap_PNG::~cBitmap_PNG(){
}

void
cBitmap_PNG::run(cContext *pCon){
	if(!mPendingAdds.empty()){
		//!\todo

		mPendingAdds.pop_back();
	}
}

cBitmap::ptrBitmap
cBitmap_PNG::getBitmap(){
	ptrBitmap rtnBM;

	//!\todo

	return rtnBM;
}
