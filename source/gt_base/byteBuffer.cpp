#include "byteBuffer.hpp"

cByteBuffer::cByteBuffer():
	mBuff(NULL),
	mBuffSize(0)
{}

cByteBuffer::~cByteBuffer(){
	if(mBuff != NULL)
		::free(mBuff);
}

const dByte*
cByteBuffer::get(const size_t pStart) const{
	if(pStart > mBuffSize || mBuff == NULL)
		throw excep::notFound("buffer, while getting", __FILE__, __LINE__);

	return &mBuff[pStart];
}

void
cByteBuffer::copy(const dByte* pBuffIn, const size_t pInSize){
	if(pBuffIn == NULL || pInSize == 0)
		throw excep::cantCopy("buffer", "empty buffer", __FILE__, __LINE__);

	if(mBuffSize != pInSize){	//- Only re-allocate if the new buffer is a different size.
		if(mBuff!=NULL)
			::free(mBuff);

		mBuff = reinterpret_cast<dByte*>(::malloc(pInSize));
		mBuffSize = pInSize;
	}

	memcpy(
		mBuff,
		pBuffIn,
		pInSize
	);
}

void
cByteBuffer::copyBuff(const cByteBuffer &pBuff){
	copy(pBuff.get(), pBuff.size());
}

void
cByteBuffer::copyBuff(const cByteBuffer* pBuff){
	copy(pBuff->get(), pBuff->size());
}

void
cByteBuffer::take(dByte* pBuffIn, size_t pInSize){
	if(pInSize == 0)
		throw excep::base_error("can't be zero, while taking", __FILE__, __LINE__);

	if(mBuff!=NULL)
		::free(mBuff);

	mBuff = pBuffIn;
	mBuffSize = pInSize;
}

void
cByteBuffer::trimHead(size_t pSize, size_t pStart){
	dByte* orig = NULL;

	if(pSize == 0)
		return;

	if(mBuffSize == 0)
		throw excep::base_error("Buffer is empty, can't be trimmed.", __FILE__, __LINE__);

	if(pStart+pSize > mBuffSize)
		throw excep::base_error("trim arguments are too big for buffer.", __FILE__, __LINE__);

	if(mBuffSize == pSize){
		clear();
	}else{
		orig = mBuff;
		mBuff = new dByte[mBuffSize - pSize];
		if(pStart > 0){
			::memcpy(mBuff, orig, pStart);
			::memcpy(&mBuff[pStart], &orig[pStart+pSize], mBuffSize - (pStart+pSize));
		}else{
			::memcpy(mBuff, &orig[pSize], mBuffSize - pSize);
		}

		mBuffSize = mBuffSize - (pStart+pSize);
		::free(orig);
	}
}


void 
cByteBuffer::clear(){
	if(mBuff!=NULL)
		::free(mBuff);

	mBuff = NULL;
	mBuffSize = 0;
}

void
cByteBuffer::add( const dByte* pBuffIn, size_t pInSize){

	dByte* oldBuff = mBuff;
	mBuff = reinterpret_cast<dByte*>(
		::malloc(mBuffSize + pInSize)
	);

	if(oldBuff != NULL){
		::memcpy(mBuff, oldBuff, mBuffSize);
		::free(oldBuff);
		::memcpy( &mBuff[mBuffSize], pBuffIn, pInSize );
	}else{
		::memcpy(mBuff, pBuffIn, pInSize);
	}
	mBuffSize += pInSize;
}

void
cByteBuffer::add(const cByteBuffer &pBuff ){
	add( pBuff.get(), pBuff.size() );
}

cByteBuffer &
cByteBuffer::operator = (const cByteBuffer &pCopyMe){
	NOTSELF(&pCopyMe);

	copyBuff(pCopyMe);
	return *this;
}

cByteBuffer &
cByteBuffer::operator+= (const cByteBuffer &pCopyMe){
	NOTSELF(&pCopyMe);

	add(pCopyMe);
	return *this;
}

//void*
//cByteBuffer::barf(const size_t pHowMuch){
//
//	if(mBuff == NULL)
//		throw excep::notFound("buffer, while barfing", __FILE__, __LINE__);
//
//	if(pHowMuch == 0 || pHowMuch > mBuffSize)
//		throw excep::base_error(__FILE__, __LINE__);	//!\todo
//
//	scrTail = reinterpret_cast<dByte*>( ::malloc(mBuffSize-pHowMuch) );
//	::memcpy( scrTail, &mBuff[pHowMuch], mBuffSize-pHowMuch );
//
//	scrHead = reinterpret_cast<dByte*>(
//		::realloc(mBuff, pHowMuch )
//	);
//
//	mBuffSize -= pHowMuch;
//	mBuff = scrTail;
//
//	return scrHead;
//}


////////////////////////////////////////////////////////////

#ifdef GTUT

GTUT_START(testByteBuffer, copy){
	cByteBuffer A;
	cByteBuffer B;
	const char *str = "derp";
	size_t strn = strlen(str);

	A.copy(reinterpret_cast<const dByte*>(str), strn);
	B.copyBuff(A);

	const char *strOut = reinterpret_cast<const char*>(B.get());
	GTUT_ASRT(strncmp(str, strOut, strn)==0, "buffer didn't copy correctly");
}GTUT_END;

GTUT_START(testByteBuffer, add){
	static const char *strA = "herp ";
	static const char *strB = "derp";
	size_t aLen = strlen(strA);
	size_t bLen = strlen(strB);
	cByteBuffer A;
	cByteBuffer B;

	A.copy(reinterpret_cast<const dByte*>(strA), aLen);
	B.copy(reinterpret_cast<const dByte*>(strB), bLen);

	A.add(B);
	const char *strOut = reinterpret_cast<const char*>(A.get());
	GTUT_ASRT(strncmp(strOut, "herp derp", 9)==0, "buffer didn't add correctly");
}GTUT_END;

GTUT_START(testByteBuffer, trimHead){
	static const char *str = "nick cave";
	cByteBuffer test;

	test.copy(reinterpret_cast<const dByte*>(str), strlen(str));
	test.trimHead(5);
	GTUT_ASRT(strncmp("cave", reinterpret_cast<const char*>(test.get()), 4)==0, "didn't trim correctly");
}GTUT_END;

GTUT_START(testByteBuffer, trimMiddle){
	static const char *str = "hungry hungry hippos";
	cByteBuffer test;

	test.copy(reinterpret_cast<const dByte*>(str), strlen(str));
	test.trimHead(7, 7);
	GTUT_ASRT(strncmp("hungry hippos", reinterpret_cast<const char*>(test.get()), 4)==0, "didn't trim correctly");
}GTUT_END;

GTUT_START(testByteBuffer, take){

}GTUT_END;

GTUT_START(testByteBuffer, fill){

}GTUT_END;

#endif
