/*
 * !\file	utils.hpp
 * !\brief	Contains all kinds of stand alone tools. This is mostly a grab bag where something will get it's own file once it matures.
 */

#ifndef UTILS_HPP
#define UTILS_HPP

#include "gt_string.hpp"
#include "exceptions.hpp"
#include <iostream>
#include <vector>
#include <map>
#include <string>
#include <sstream>

#ifdef GTUT
	#include "unitTests.hpp"
#endif

using namespace std;

//------------------------------------------------------------------------------------------
//!\brief	useful for those odd occasions when you need a parameter in a function 
//			interface, but don't want to use it in a given implementation.
#ifdef DEBUG
	#define DUMB_REF_ARG(A) (void)A;
#else
	#define DUMB_REF_ARG(A)
#endif

//------------------------------------------------------------------------------------------
#define SAFEDEL(P) if(P != NULL){ delete(P); P=NULL; }


//------------------------------------------------------------------------------------------
// some defines which are common in this project.
typedef unsigned int	dNameHash;
typedef unsigned int	dMillisec;

//------------------------------------------------------------------------------------------
//!\note	This should return the same hash for every platform, so
//!			it has to use the portable char type.
// !\note	Code taken from http://cboard.cprogramming.com/tech-board/114650-string-hashing-algorithm.html
dNameHash makeHash(const char* pString);

//------------------------------------------------------------------------------------------
// Handy if you don't want to expose the container, but you
// want access to its elements.

template <typename T>
class tCoolItr{
public:

	tCoolItr(const T* pContainer, typename T::const_iterator pStartHere) :
		mContainerRef(pContainer), mCurrentSpot(pStartHere)
	{}

	tCoolItr(const T* pContainer) :
		mContainerRef(pContainer), mCurrentSpot(mContainerRef->begin())
	{}

	void operator ++ () {
		if(stillGood)
			++mCurrentSpot;
	}

	bool stillGood() {
		if(mCurrentSpot != mContainerRef->end())
			return true;

		return false;
	}

	typename T::value_type get(){
		return *mCurrentSpot;
	}

	void reset(){
		mCurrentSpot = mContainerRef->begin();
	}

private:
	const T* mContainerRef;
	typename T::const_iterator mCurrentSpot;
};

//------------------------------------------------------------------------------------------
//!\brief	Handy little function for just seeing if a value is in a vector.
template<typename T>
bool
isIn(const T &pFindMe, const vector<T> &pFindIn){

	//for(size_t idx = 0; idx < pFindIn.size(); ++idx){
	//	if(pFindMe == pFindIn[idx]){
	//		return true;
	//	}
	//}
	//return false;

	for(typename vector<T>::const_iterator itr = pFindIn.begin(); itr != pFindIn.end(); ++itr){
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

	typedef map<dNameHash, sItem> dItemMap;
	typedef pair<dNameHash, sItem> dItemPair;

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

	//- Above is the same, but it's cool because it takes advantage of && operator returning bool.
	//	if(i>0 && (i & (i - 1)) == 0)
	//		return true;
	//	return false;
}

//------------------------------------------------------------------------------------------
typedef unsigned short dIDSLookup;	//!< index into a short lookup table.

//!\brief	Stores a smaller number of elements (65,535 which should be enough), and keeps track of holes, refilling them when needed.
template<typename T>
class tShortLookup{
public:
	tShortLookup();
	~tShortLookup();

	T get(dIDSLookup aID);

	T* getRaw();	//!< Be careful with this. Intended for really fast lookups where we're sure we can't go out of bounds or use a free-ed slot.

	dIDSLookup add(T &aData);

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

//------------------------------------------------------------------------------------------
//!\brief	Has sole ownership of a pointer which it cleans up and allows child classes
//!			to work with.
template<typename T>
class tPMorphJar{
private:
	T *data;

public:
	explicit tPMorphJar() : data(NULL){}

	template<typename COPY> tPMorphJar(const COPY &copyMe){
		data = new COPY();
		*data = copyMe;
	}

	template<typename COPY> tPMorphJar(const tPMorphJar<COPY> &copyMe){
		data = new COPY();
		*data = *copyMe.data;
	}

	~tPMorphJar(){ delete data; }

	tPMorphJar<T> & operator = (const tPMorphJar<T> &otherJar){
		if(&otherJar != this){
			*data = *otherJar.data;
		}
		return *this;
	}

	template<typename COPY> tPMorphJar<T> & operator = (const COPY &copyMe){
		delete data;
		data = new COPY();
		*data = copyMe;
		return *this;
	}

	T& get(){ return *data; }
};

////////////////////////////////////////////////////////////
// Template definitions

template<typename T>
tShortLookup<T>::tShortLookup(): table(NULL), size(0), numFree(0){}

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
dIDSLookup
tShortLookup<T>::add(T &aData){
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
