/*
 * !\file	byteBuffer.hpp
 * !\brief	Interface for the gametool's byte buffer type.
 *
 */

#ifndef BYTEBUFFER_HPP
#define BYTEBUFFER_HPP

#include "utils.hpp"
#include "exceptions.hpp"
#include <stdlib.h>	//for memory ops.

#if CHAR_BIT == 8
	typedef char			dByte;		//!< This is the gametool's most basic byte type. It is always 8 bits.
#else
	#error the byte buffer is not 8 bits, and I'm too lazy to write something for your environment to enforce 8 bit buffers.
#endif

//!\class	cByteBuffer
//!\brief	8Bit buffer used for consistency across platforms and not for performance.
//!\note	The gamestool byte buffer is always 8bits even if it is not effective for the system to process 8 bit buffers. This ensures that data files
//!			are compatible across the different gamestool virtual machines.
//!\note	The gamestool should pretty much only use byte buffers for saving and loading data, because the guarntee of 8bit buffers is more important
//!			than performance.
//!\todo	Make a trick where, if the buffer is being trimmed from the head, it's not trimmed but only offset. Only when the buffer is manipulated is
//!			the trim actually applied. That way, frequent commands to trim the head off a buffer don't result in re-allocation every time.
class cByteBuffer{

public:
	cByteBuffer();
	cByteBuffer(cByteBuffer& pCopy){
		copyBuff(pCopy);
	}
	~cByteBuffer();

	size_t size() const{ return mBuffSize; }
	void clear();

	const dByte* get(const size_t pStart=0) const;
	
	template<typename TYPE> void fill(TYPE* pCup, size_t pStart=0) const{
		if( pStart + sizeof(TYPE) > mBuffSize )
			throw excep::base_error("", __FILE__, __LINE__);
	
		::memcpy( static_cast<void*>(pCup), static_cast<void*>(&mBuff[pStart]), sizeof(TYPE) );
	}	//!< Fill the target type with what's in the buffer (if there is enough to room in the buffer to fill the target). Can begin at an offset into the buffer.

	void copy(const dByte* pBuffIn, size_t pInSize);	//!< This buffer will free its current contents (if the size is different), and copy what's being pointed too.
	void copyBuff(const cByteBuffer &pBuff);
	void copyBuff(const cByteBuffer *pBuff);
	template<typename TYPE> void copy(TYPE* pBuffIn){ copy( (dByte*)(pBuffIn), sizeof(TYPE) ); }

	void take(dByte* pBuffIn, size_t pInSize);	//!< Buffer takes a memory location, which makes it responsible for cleaning it up later.
	template<typename TYPE> void take(TYPE* pBuffIn){
		take( (dByte*)(pBuffIn), sizeof(TYPE) );
	}	//!< Slack way of using take.
	
	void trim(size_t pSize, size_t pStart=0);	//!< Deletes a section of the buffer and recombines it if there are 2 halves remaining.

	void add( const dByte* pBuffIn, size_t pInSize);
	void add( const cByteBuffer &pBuff ); //!< Copies itself and the content being pointed to into a new buffer that combines the two.
	template<typename TYPE> void add(TYPE* pBuffIn){ add( (dByte*)(pBuffIn), sizeof(TYPE) ); }

	cByteBuffer & operator= (const cByteBuffer &pCopyMe);

protected:
	dByte*	mBuff;
	size_t	mBuffSize;
	//	dByte*	scrHead;
	//	dByte*	scrTail;
};


#endif
