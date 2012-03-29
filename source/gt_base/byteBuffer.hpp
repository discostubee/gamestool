/*
 * !\file	byteBuffer.hpp
 * !\brief	Interface for the gametool's byte buffer type.
 *
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

#ifndef BYTEBUFFER_HPP
#define BYTEBUFFER_HPP

#include "binPacker.hpp"
#include "exceptions.hpp"
#include <stdlib.h>	//for memory ops.

#ifdef DEBUG
	#include <string.h> // c style string tools.

	// Use this to turn on buffer tracking, a kludgy way to determine if there is a memory leak.
	#define TRACK_BUFF
#endif


//!\brief	8Bit buffer used for consistency across platforms and not for performance. If you give it memory location to a type, it will store
//!			it as a cross platform binary stream possibly not exactly the same as the memory you passed it.
//!\note	The gamestool byte buffer is always 8bits even if it is not effective for the system to process 8 bit buffers. This ensures that data files
//!			are compatible across the different gamestool virtual machines.
//!\note	The gamestool should pretty much only use byte buffers for saving and loading data, because the guarantee of 8bit buffers is more important
//!			than performance.
//!\todo	Make a trick where, if the buffer is being trimmed from the head, it's not trimmed but only offset. Only when the buffer is manipulated is
//!			the trim actually applied. That way, frequent commands to trim the head off a buffer doesn't result in re-allocation every time.
//!\todo	Make a trick where appends (take, add) to the buffer are kept as referenced chunks, thus avoiding having to resize and recopy the entire buffer.
//!			The buffer is only compacted into one complete stream when a get request is made. This should make appends much faster because quite often
//!			we want to make heaps of appends and then read a complete stream at the end. The only downside would be that we are effectivly stalling the
//!			performance hit we get when resizing the buffer.
class cByteBuffer{

public:
	cByteBuffer();
	cByteBuffer(const cByteBuffer& pCopy){
		copyBuff(pCopy);
	}
	~cByteBuffer();

	size_t size() const{ return mBuffSize; }
	void clear();

	const dByte* get(const size_t pStart=0) const;
	
	template<typename TYPE> void fill(TYPE *pCup, size_t pStart=0) const;	//!< Fill the target type based upon the memory found at the start location. It unpacks the binary data it has into the type requested.

	void copy(const dByte* pBuffIn, size_t pInSize);	//!< This buffer will free its current contents (if the size is different), and copy what's being pointed too.
	template<typename TYPE> void copy(const TYPE *pBuffIn);

	void copyBuff(const cByteBuffer &pBuff);
	void copyBuff(const cByteBuffer *pBuff);

	void take(dByte* pBuffIn, size_t pInSize);	//!< Buffer takes a memory location, which makes it responsible for cleaning it up later.
	template<typename TYPE> void take(TYPE *pBuffIn);	//!< Slack way of using take.
	
	void trimHead(size_t pSize, size_t pStart=0);	//!< Deletes a section, or just from the start, of the buffer and recombines it if there are 2 halves remaining.

	void add( const dByte* pBuffIn, size_t pInSize);
	void add( const cByteBuffer &pBuff ); //!< Copies itself and the content being pointed to into a new buffer that combines the two.
	template<typename TYPE> void add(const TYPE *pBuffIn);

	cByteBuffer & operator= (const cByteBuffer &pCopyMe);	//!< Alias for copy.
	cByteBuffer & operator+= (const cByteBuffer &pCopyMe);	//!< Alias for add.

	class excepUnderFlow: public excep::base_error{
	public:
		excepUnderFlow(const char* pFunc, const unsigned int pLine) throw():
            base_error(pFunc, pLine)
         { addInfo("buffer underflow"); }
		virtual ~excepUnderFlow() throw(){}
	};

	class excepOverFlow: public excep::base_error{
		excepOverFlow(const char* pFunc, const unsigned int pLine) throw():
			base_error(pFunc, pLine)
		 { addInfo("buffer overflow"); }
		virtual ~excepOverFlow() throw(){}
	};

protected:
	dByte*	mBuff;
	size_t	mBuffSize;
};


template<typename TYPE>
void
cByteBuffer::fill(TYPE *pCup, size_t pStart) const{
	ASRT_NOTNULL(pCup);

	int sizeUnpacked=0;
	bpk::unpack<TYPE>(&mBuff[pStart], pCup, &sizeUnpacked, mBuffSize-pStart);
	if(sizeUnpacked != sizeof(TYPE) || pCup == NULL)
		throw excepUnderFlow(__FILE__, __LINE__);
}

template<typename TYPE>
void
cByteBuffer::copy(const TYPE *pBuffIn){
	ASRT_NOTNULL(pBuffIn);

	delete mBuff;
	mBuff = NULL;
	mBuffSize = 0;
	bpk::pack<TYPE>(pBuffIn, &mBuff, &mBuffSize, sizeof(TYPE));
}

template<typename TYPE>
void
cByteBuffer::take(TYPE *pBuffIn){
	ASRT_NOTNULL(pBuffIn);

	delete mBuff;
	mBuff=NULL;
	mBuffSize=0;
	bpk::pack<TYPE>(pBuffIn, &mBuff, &mBuffSize, sizeof(TYPE));
	delete pBuffIn;
}

template<typename TYPE>
void
cByteBuffer::add(const TYPE *pBuffIn){
	ASRT_NOTNULL(pBuffIn);

	dByte *packed=NULL;
	int sizePacked=0;
	bpk::pack<TYPE>(pBuffIn, &packed, &sizePacked, mBuffSize);
	add( packed, sizePacked );
	delete packed;
}

#endif
