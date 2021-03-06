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

#include <stdlib.h>	//for memory ops.

#ifdef DEBUG
	#include <string.h> // c style string tools.

	// Use this to turn on buffer tracking, a kludgy way to determine if there is a memory leak.
	#define TRACK_BUFF
#endif

///////////////////////////////////////////////////////////////////////////////////
//!\brief	8Bit buffer used for consistency across platforms and not for performance. If you give it a memory location to a type, it will store
//!			it as a cross platform binary stream.
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
	cByteBuffer(const cByteBuffer& pCopy);
	~cByteBuffer();

	size_t size() const{ return mBuffSize; } //!< Returns the total byte size of the buffer.
	void clear();

	const dByte* get(const size_t pStart=0) const;
	
	template<typename TYPE> int fill(TYPE *pCup, size_t pStart=0) const;	//!< Fill the target based upon its TYPE using memory at the head of the buffer offset by pStart. !\return The bytes used to fill (NOT ALWAYS the same as the byte size of the container) !\note DOES NOT call the TYPE's constructor, so be careful with. Returns the number of bytes used in filling (not always the number of bytes the object is).
	template<typename TYPE> int arrFill(TYPE pArr[], size_t pLen, size_t pStart=0) const;	//!< Fills of a given TYPE similar to 'fill' function above.

	void copy(const dByte* pBuffIn, size_t pInSize);	//!< This buffer will free its current contents (if the size is different), and copy what's being pointed too.
	template<typename TYPE> void copy(const TYPE *pBuffIn);

	void copyBuff(const cByteBuffer &pBuff);
	void copyBuff(const cByteBuffer *pBuff);

	void take(dByte* pBuffIn, size_t pInSize);	//!< Buffer takes a memory location, which makes it responsible for cleaning it up later.
	template<typename TYPE> void take(TYPE *pBuffIn);	//!< Slack way of using take.
	
	void trimHead(size_t pSize, size_t pStart=0);	//!< Deletes a section, or just from the start, of the buffer and recombines it if there are 2 halves remaining.

	void add(const dByte* pBuffIn, size_t pInSize);
	void add(const cByteBuffer &pBuff); //!< Copies itself and the content being pointed to into a new buffer that combines the two.
	template<typename TYPE> void add(const TYPE *pIn);	//!< Copies the TYPE and adds it to the end of the buffer. Should be able to use this memory to refil a container of the same TYPE.

	void overwrite(const dByte *pWriteWith, size_t pWriteSize, size_t pStart = 0);	//!< Overwrite part of the buffer with another one. Throws if you try to write past what the current buffer contains.

	cByteBuffer & operator= (const cByteBuffer &pCopyMe);	//!< Alias for copy.
	cByteBuffer & operator+= (const cByteBuffer &pCopyMe);	//!< Alias for add.
	bool operator== (const cByteBuffer &pOther) const;	//!< Compares start memory address, for now.

protected:
	dByte*	mBuff;
	size_t	mBuffSize;
};

///////////////////////////////////////////////////////////////////////////////////

template<typename TYPE>
int
cByteBuffer::fill(TYPE *pCup, size_t pStart) const{
	ASRT_NOTNULL(pCup);

	size_t sizeUnpacked=0;
	bpk::unpack(pCup, &mBuff[pStart], &sizeUnpacked, mBuffSize-pStart);
	return sizeUnpacked;
}

template<typename TYPE>
int
cByteBuffer::arrFill(TYPE pArr[], size_t pLen, size_t pStart) const{
	ASRT_TRUE(pLen > 0, "Length must not be zero.");

	size_t sizeUnpacked=0;
	for(size_t i=0; i < pLen; ++i){
		bpk::unpack(
			&pArr[i],
			&mBuff[pStart + (i*sizeUnpacked)],
			&sizeUnpacked,
			mBuffSize -pStart -(i*sizeUnpacked)
		);
		ASRT_TRUE(sizeUnpacked > 0, "Unpacked 0.");
	}
	return sizeUnpacked * pLen;
}

template<typename TYPE>
void
cByteBuffer::copy(const TYPE *pBuffIn){
	ASRT_NOTNULL(pBuffIn);

	delete [] mBuff;
	mBuff = NULL;
	mBuffSize = 0;
	bpk::pack(pBuffIn, &mBuff, &mBuffSize, sizeof(TYPE));
}

template<typename TYPE>
void
cByteBuffer::take(TYPE *pBuffIn){
	ASRT_NOTNULL(pBuffIn);

	delete mBuff;
	mBuff=NULL;
	mBuffSize=0;
	bpk::pack(pBuffIn, &mBuff, &mBuffSize, sizeof(TYPE));
	delete [] pBuffIn;
}

template<typename TYPE>
void
cByteBuffer::add(const TYPE *pIn){
	ASRT_NOTNULL(pIn);

	dByte *packed=NULL;
	size_t sizePacked=0;
	bpk::pack(pIn, &packed, &sizePacked);
	add( packed, sizePacked );
	delete [] packed;
}

#endif
