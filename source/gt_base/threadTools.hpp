/*
 * !\file	threadTools.hpp
 * !\brief
 *
 *  Copyright (C) 2010  Stuart Bridgens
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#ifndef THREADTOOLS_HPP
#define THREADTOOLS_HPP

//!\brief	Define this in your project settings if you want to compile threadding. You'll need to include the compiled
//!			boost library if you do.
//#define GT_THREADS

#include "ptrTools.hpp"
#include "utils.hpp"

#ifdef GT_THREADS
#	include <boost/thread/locks.hpp>
#	include <boost/thread.hpp>
#endif

//-------------------------------------------------------------------------------------
#ifdef GT_THREADS
#	define CRITSEC static boost::mutex mu; boost::unique_lock<boost::mutex> lock(mu)
#else
#	define CRITSEC
#endif

namespace gt{

	template<typename T> class tMrSafety;

	//-------------------------------------------------------------------------------------
	//!\brief	Because this is a read only functions, and because this
	//!			is updated once before starting threads, it doesn't need to be mutex locked
	class isMultithreading{
#		ifdef GT_THREADS
			public:
				static bool yes() { return xThreading; }
				static void nowThreading() { xThreading = true; }
			private:
				static bool xThreading;
#		else
			public:
				static bool yes() { return false; }
				static void nowThreading() {}
#		endif
	};

	//-------------------------------------------------------------------------------------
	//!\brief	Handle to a resource kept by mr safety, who is told when this handle dies.
	template<typename T>
	class tSafeLem{
	public:
		tSafeLem(tMrSafety<T> *mr);
		tSafeLem(const tSafeLem<T> &lem);
		~tSafeLem<T>();

		T* get ();
		T* operator -> ();
		tSafeLem<T>& operator = (const tSafeLem<T> &copy);

	protected:
		tMrSafety<T> *mParent;

	friend class tMrSafety<T>;
	};

	//-------------------------------------------------------------------------------------
	//!\brief	Replicates the lemming spitter. Would have been nice to keep it as a base
	//!			class, but mr safety really needs to be its own thing.
	template<typename T>
	class tMrSafety{
	public:
		tMrSafety();
		~tMrSafety();

		void take(T* takeMe); 		//!< Cleans up the old data if it exists, and becomes the custodian of the data being past in. Waits to acquire lock.
		void cleanup();				//!< Deletes data. Waits to acquire lock.
		void drop(); 				//!< Don't manage this anymore. Doesn't cleanup. Waits to acquire lock.
		void set(const T& data);	//!< Set containing data and deletes any old data. Requires lock.
		tSafeLem<T> get();

	protected:
#		ifdef GT_THREADS
			boost::recursive_mutex muData;
			typedef boost::lock_guard<boost::recursive_mutex> dMuLock;
#		endif

		void deadLemming();	//!<	If this is the last lemming for this thread, release the lock for next thread in queue.
		void changedLem(tSafeLem<T>* from, tSafeLem<T>* to);	//!< Makes the 'from' lemming empty and makes the 'to' lemming linked to this manager.
		T* getLockData(tSafeLem<T>* requester);	//!<	Acquires lock for current thread. Blocks and waits to acquire lock if request comes from another thread.

	private:
		T* mData;
		int inWild;	//!< Gotta keep track of the number of times we use the lock.

	friend class tSafeLem<T>;
	};
}

///////////////////////////////////////////////////////////////////////////////////
// Templates
namespace gt{
//-------------------------------------------------------------------------------------
template<typename T>
tSafeLem<T>::tSafeLem(tMrSafety<T> *mr) :
	mParent(mr)
{
}

template<typename T>
tSafeLem<T>::tSafeLem(const tSafeLem<T> &lem) :
	mParent(lem.mParent)
{
	mParent->changedLem(const_cast<tSafeLem*>(&lem), this);
}

template<typename T>
tSafeLem<T>::~tSafeLem(){
	if(mParent!=NULL)
		mParent->deadLemming();
}


template<typename T> T*
tSafeLem<T>::get () {
	if(mParent==NULL)
		return NULL;

	return mParent->getLockData(this);
}

template<typename T>
T*
tSafeLem<T>::operator -> () {
	return get();
}

template<typename T>
tSafeLem<T>&
tSafeLem<T>::operator = (const tSafeLem &copy){
	ASRT_NOTSELF(&copy);

	mParent->changedLem(static_cast<tSafeLem*>(&copy), this);

	return *this;
}


//-------------------------------------------------------------------------------------

template<typename T>
tMrSafety<T>::tMrSafety() :
	mData(NULL), inWild(0)
{}

template<typename T>
tMrSafety<T>::~tMrSafety()
{
	try{
		cleanup();
	}catch(...){
	}
}

template<typename T>
void
tMrSafety<T>::take(T* takeMe) {
#	ifdef GT_THREADS
		dMuLock lock(muData);
#	endif

	mData = takeMe;
}

template<typename T>
void
tMrSafety<T>::cleanup() {
#	ifdef GT_THREADS
		dMuLock lock(muData);
#	endif
	SAFEDEL(mData);
}

template<typename T>
void
tMrSafety<T>::drop() {
#	ifdef GT_THREADS
		dMuLock lock(muData);
#	endif

	mData = NULL;
}

template<typename T>
void
tMrSafety<T>::set(const T& data) {
#	ifdef GT_THREADS
		dMuLock lock(muData);
#	endif

	delete mData;
	mData = new T;
	*mData = data;
}

template<typename T>
tSafeLem<T>
tMrSafety<T>::get() {
#	ifdef GT_THREADS
		muData.lock();
#	endif

	++inWild;

	return tSafeLem<T>(this);
}

template<typename T>
void
tMrSafety<T>::deadLemming(){
	ASRT_TRUE(inWild > 0, "Lemming underflow.");

	--inWild;

#	ifdef GT_THREADS
		muData.unlock();
#	endif
}

template<typename T>
void
tMrSafety<T>::changedLem(tSafeLem<T>* from, tSafeLem<T>* to){
	ASRT_TRUE(from->mParent == this, "Tried to change from a lemming that didn't come from this manager.");

	if(to->mParent != this){
		--to->mParent->inWild;

#		ifdef GT_THREADS
			to->mParent->muData.unlock();
			muData.lock();	//- add to lock again
#		endif
	}

	to->mParent = this;
	from->mParent = NULL;
}

template<typename T>
T*
tMrSafety<T>::getLockData(tSafeLem<T>* requester){
	ASRT_TRUE(requester->mParent == this, "A lemming requested data from the wrong manager");
	ASRT_NOTNULL(mData);
	return mData;
}


}

#endif
