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
dNameHash makeHash(const dNatChar* pString);

//------------------------------------------------------------------------------------------
//!\class	cGlobalPtrMan
//!\brief	Global pointer manager. Used to delete a global pointer.
template<typename TYPE>
class cGlobalPtrMan{
public:
	TYPE* mPtr;

	cGlobalPtrMan(TYPE* pPtr):mPtr(pPtr){}
	~cGlobalPtrMan(){ SAFEDEL(mPtr); }
};

//------------------------------------------------------------------------------------------
// Dimension stuff. Should go in its own source file.

typedef float dUnitVD;	//!< Virtual distance measurement.
typedef float dUnitVA;	//!< Virtual angle measurement.

//!\brief Three Dimensional vector
struct s3DVec{
	dUnitVD	scaler;
	dUnitVA u, v, w;	//!< Rotation angle from parent axis.
};

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
//!\brief	Makes using statics cool again. Used when you can't be sure of when a global will
//!			be initialised.
template<typename T>
class cCoolStatic{
private:
	static T* mD;

public:

	cCoolStatic(){
		std::cout << "cool static made at " << (long)(this) << " as a " << typeid(T).name() << std::endl; //!!!
	}

	~cCoolStatic(){
		setupOrCleanup(false);
	}

	static void setupOrCleanup(bool pDontCleanup=true){
		static bool setup = false;

		if(pDontCleanup){
			if(!setup){
				mD = new T();
				setup = true;
				std::cout << "cool static got memory at " << (long)mD << " for " << typeid(T).name() << std::endl; //!!!
			}
		}else{
			if(setup){
				std::cout << "cool static about to clean memory at " << (long)mD << " for " << typeid(T).name() << std::endl; //!!!
				delete mD;
				mD = NULL;
				setup = false;
			}
		}
	}

	static T* get(){
		setupOrCleanup();
		return mD;
	}

	static void set(T* pSetter){
		setupOrCleanup(false);
		mD = pSetter;
	}

	//!\brief	Used when we don't want this static to manage the pointer anymore.
	static void drop(){
		mD = NULL;
		setupOrCleanup(false);
	}
};

// Don't assign anything here.
template<typename T>
T* cCoolStatic<T>::mD;


//------------------------------------------------------------------------------------------
//!\brief Useage tracker. Helpful for tracing possibly memory leaks.
class cTracker{
public:
	static void start(const dNatChar* pName);
	static void stop(const dNatChar* pName);
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

	//- Above is really, but it's cool because it takes advantage of && operator returning bool.
	//	if(i>0 && (i & (i - 1)) == 0)
	//		return true;
	//	return false;
}


#endif
