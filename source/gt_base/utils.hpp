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
 * !\file	utils.hpp
 * !\brief	Contains all kinds of stand alone tools. This is mostly a grab bag where something will get it's own file once it matures.
 */

#ifndef UTILS_HPP
#define UTILS_HPP

#include "exceptions.hpp"
#include <iostream>
#include <vector>
#include <map>
#include <string>	// c++ library
#include <string.h> // C library for memcpy.
#include <sstream>

#ifdef GTUT
	#include "unitTests.hpp"
#endif

//------------------------------------------------------------------------------------------
//!\brief	useful for those odd occasions when you need a parameter in a function 
//			interface, but don't want to use it in a given implementation.
#ifdef DEBUG
	#define DUMB_REF_PAR(A) (void)A;
#else
	#define DUMB_REF_PAR(A)
#endif

//------------------------------------------------------------------------------------------
#define SAFEDEL(P) {delete P; P=NULL;}
#define SAFEDEL_ARR(P) {delete [] P; P=NULL;}


//------------------------------------------------------------------------------------------
#if defined(__APPLE__)
#	define SHITPANTS abort()
#elif defined(__linux)
#	define SHITPANTS abort()
#endif
//------------------------------------------------------------------------------------------
// some defines which are common in this project.
typedef unsigned int	dHash;
typedef unsigned int	dNameHash;
typedef unsigned int	dMillisec;

static const dNameHash HASH_INVALID = -1;

#if CHAR_BIT == 8 || __CHAR_BIT__ == 8
	typedef char		dByte;		//!< This is the gametool's most basic byte type. It is always 8 bits.
#else
	#error "the byte buffer is not 8 bits, and I'm too lazy to write something for your environment to enforce 8 bit buffers."
#endif

//------------------------------------------------------------------------------------------
// !\note	Code taken from http://cboard.cprogramming.com/tech-board/114650-string-hashing-algorithm.html
dHash makeHash(const char *pString);	//!< Make hash from literal string
dNameHash makeHash(const dNatStr &pString);	//!< Should be used when dealing with a hash identifier that we need to be consistent across platforms.

//------------------------------------------------------------------------------------------
//!\brief	Handy if you don't want to expose the container, but you want access to its elements.
template <typename T>
class tCoolItr{
public:

	tCoolItr(T* pContainer, typename T::iterator pStartHere)
	: mContainerRef(pContainer), mCurrentSpot(pStartHere)
	{}

	tCoolItr(T* pContainer)
	: mContainerRef(pContainer), mCurrentSpot(mContainerRef->begin())
	{}

	tCoolItr(const tCoolItr &copyMe)
	: mContainerRef( const_cast<T*>(copyMe.mContainerRef) )
	{}

	bool stillGood() {
		if(mCurrentSpot != mContainerRef->end())
			return true;

		return false;
	}

	typename T::value_type& get(){
		return *mCurrentSpot;
	}

	void reset(){
		mCurrentSpot = mContainerRef->begin();
	}

	void operator ++ () {
		if(stillGood())
			++mCurrentSpot;
	}

	typename T::value_type& operator* (){
		return get();
	}

private:
	T* mContainerRef;	//!< Don't delete
	typename T::iterator mCurrentSpot;
};

//------------------------------------------------------------------------------------------
//!\brief	Handy if you don't want to expose the map, or if you don't want to get the
//!			container again to check the iterator against it.
template <typename KEY, typename T>
class tCoolFind{
public:
	typedef std::map<KEY, T> dMap;

	tCoolFind(dMap &aMap, KEY aFindMe) :
		refMap(aMap), mFind(aMap.find(aFindMe))
	{}

	bool found(){
		return mFind != refMap.end();
	}

	T& get(){
		return mFind->second;
	}

private:
	dMap &refMap;
	typename dMap::iterator mFind;
};

//------------------------------------------------------------------------------------------
//!\brief	Handy little function for just seeing if a value is in a vector.
template<typename T>
bool
isIn(const T &pFindMe, const std::vector<T> &pFindIn){

	//for(size_t idx = 0; idx < pFindIn.size(); ++idx){
	//	if(pFindMe == pFindIn[idx]){
	//		return true;
	//	}
	//}
	//return false;

	for(typename std::vector<T>::const_iterator itr = pFindIn.begin(); itr != pFindIn.end(); ++itr){
		if(*itr == pFindMe){
			return true;
		}
	}
	return false;
}


//------------------------------------------------------------------------------------------
//!\brief	Useage tracker. Helpful for tracing possibly memory leaks.
//!\todo	Make this threadsafe.
class cTracker{
public:
	static void start(const char* pName);
	static void stop(const char* pName);
	static void makeReport(std::ostream &report);

protected:
	static void ensureSetup(bool setup=true);

private:

	struct sItem{
		dStr	mName;
		int		mCount;
	};

	class cAutoCleanup{
	public:
		cAutoCleanup(){}
		~cAutoCleanup();

		friend class cTracker;
	};

	typedef std::map<dNameHash, sItem> dItemMap;
	typedef std::pair<dNameHash, sItem> dItemPair;

	static dItemMap* xObjectsActive;
	static dItemMap::iterator xItemItr;
	static dNameHash xTempHash;
	static cAutoCleanup xCleanup;

};

#ifdef DEBUG
	#define DBUG_TRACK_START(x)	cTracker::start(x)
	#define DBUG_TRACK_END(x)	cTracker::stop(x)
#else
	#define DBUG_TRACK_START(x)
	#define DBUG_TRACK_END(x)
#endif

//-------------------------------------------------------
//!\brief	if 2^number then true.
//!\note	5 finger discount at the http://www.parashift.com/c++-faq-lite/intrinsic-types.html store.
inline bool
isPowerOf2(int i)
{
	return i > 0 && (i & (i - 1)) == 0;

	//- Above is the same, but it's cooler because it takes advantage of && operator returning bool.
	//	if(i>0 && (i & (i - 1)) == 0)
	//		return true;
	//	return false;
}

//------------------------------------------------------------------------------------------
typedef unsigned int dIDSLookup;	//!< index into a short lookup table. Not nested in class because it's a template.
static const dIDSLookup SL_NO_ENTRY = static_cast<dIDSLookup>(-1);

//!\brief	Stores a smaller number of elements (65,535 which should be enough), and keeps track of holes, refilling them when needed.
template<typename T>
class tShortLookup{
public:

	tShortLookup();
	~tShortLookup();

	T get(dIDSLookup aID);

	T* getRaw();	//!< Be careful with this. Intended for really fast lookups where we're sure we can't go out of bounds or use a free-ed slot.

	bool valid(dIDSLookup aID);	//!< True if the ID is in range and the slot is not free, in other words is this slot valid.

	dIDSLookup add(const T &aData);

	void del(dIDSLookup aID);

private:
	struct slot{
		bool free;
		T data;
	};

	slot* table;
	slot* itr;
	unsigned short size;
	unsigned short numFree;
};

////////////////////////////////////////////////////////////
// Template definitions

template<typename T>
tShortLookup<T>::tShortLookup(): table(NULL), itr(NULL), size(0), numFree(0) {}

template<typename T>
tShortLookup<T>::~tShortLookup(){
	delete [] table;
}

template<typename T>
T
tShortLookup<T>::get(dIDSLookup aID){
	if(aID > size)
		throw excep::base_error("ID outside range", __FILE__, __LINE__);

	itr = &table[aID];

	if(itr->free)
		throw excep::base_error("trying to get a free slot", __FILE__, __LINE__);

	return itr->data;
}

template<typename T>
T*
tShortLookup<T>::getRaw(){
	return table;
}

template<typename T>
bool
tShortLookup<T>::valid(dIDSLookup aID){
	if(aID > size)
		return false;

	return !table[aID].free;
}

template<typename T>
dIDSLookup
tShortLookup<T>::add(const T &aData){
	if(numFree == 0){
		slot* tmp = new slot[size + 1];
		::memcpy(tmp, table, size * sizeof(T));
		delete [] table;
		table = tmp;
		itr = &table[size];
		++size;

		itr->free = false;
		itr->data = aData;

		return (size - 1);
	}else{
		for(dIDSLookup idx=0; idx < size; ++idx){
			itr = &table[idx];
			if(itr->free){
				itr->data = aData;
				itr->free = false;
				--numFree;
				return idx;
			}
		}
		throw excep::base_error("no free slots when some were reported", __FILE__, __LINE__);	//- just in case
	}

}

template<typename T>
void
tShortLookup<T>::del(dIDSLookup aID){
	if(aID > size)
		throw excep::base_error("ID outside range", __FILE__, __LINE__);

	table[aID].free = true;
	++numFree;
}



#endif
