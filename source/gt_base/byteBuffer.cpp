/*
 **********************************************************************************************************
 *  Copyright (C) 2010  Stuart Bridgens
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License (version 3) as published by
 *  the Free Software Foundation.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *********************************************************************************************************
*/

#include "byteBuffer.hpp"
#include "exceptions.hpp"

cByteBuffer::cByteBuffer():
	mBuff(NULL),
	mBuffSize(0)
{}

cByteBuffer::cByteBuffer(const cByteBuffer& pCopy):
	mBuff(NULL),
	mBuffSize(0)
{
	copyBuff(pCopy);
}

cByteBuffer::~cByteBuffer(){
	delete [] mBuff;
}

const dByte*
cByteBuffer::get(const size_t pStart) const{
	if(pStart > mBuffSize)
		throw excep::underFlow(__FILE__, __LINE__);

	if(mBuff == NULL)
		return NULL;

	return &mBuff[pStart];
}

void
cByteBuffer::copy(const dByte* pBuffIn, const size_t pInSize){
	if(pBuffIn == NULL || pInSize == 0){
		delete [] mBuff;
		mBuff = NULL;
		mBuffSize = 0;
		return;
	}

	if(mBuffSize != pInSize){	//- Only re-allocate if the new buffer is a different size.
		delete [] mBuff;
		mBuff = new dByte[pInSize];
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

	delete [] mBuff;

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
		delete [] orig;
	}
}


void 
cByteBuffer::clear(){
	if(mBuff!=NULL)
		delete [] mBuff;

	mBuff = NULL;
	mBuffSize = 0;
}

void
cByteBuffer::add( const dByte* pBuffIn, size_t pInSize){
	dByte* oldBuff = mBuff;
	mBuff = new dByte[pInSize+mBuffSize];

	if(oldBuff != NULL){
		::memcpy(mBuff, oldBuff, mBuffSize);
		delete [] oldBuff;
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

void
cByteBuffer::overwrite(const dByte *pWriteWith, size_t pWriteSize, size_t pStart){
	if(pStart + pWriteSize > mBuffSize)
		excep::overFlow(__FILE__, __LINE__);

	memcpy(&mBuff[pStart], pWriteWith, pWriteSize);
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

bool
cByteBuffer::operator== (const cByteBuffer &pOther) const{
	return (mBuff == pOther.mBuff);
}


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


#endif
