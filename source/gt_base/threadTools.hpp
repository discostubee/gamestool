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
	#include <boost/thread/locks.hpp>
	#include <boost/thread.hpp>
#endif

namespace gt{

#ifdef GT_THREADS
	typedef boost::thread::id dThreadID;
	typedef boost::unique_lock<boost::mutex> dLock;
#endif

	template<typename T> class tMrSafety;

	//-------------------------------------------------------------------------------------
	//!\brief	Because this is a read only functions, and because this
	//!			is updated once before starting threads, it doesn't need to be mutex locked
	class isMultithreading{
#ifdef GT_THREADS
	public:
		static bool yes() { return xThreading; }
		static void nowThreading() { xThreading = true; }
	private:
		static bool xThreading;
#else
	public:
		static bool yes() { return false; }
		static void nowThreading() {}
#endif
	};

	//-------------------------------------------------------------------------------------
	//!\brief	Handle to a resource kept by mr safety, who is told when this handle dies.
	template<typename T>
	class tSafeLem{
	public:
		tSafeLem(tMrSafety<T> *mr);
		tSafeLem(tSafeLem<T> &lem);
		~tSafeLem<T>();

		T* get ();
		T* operator -> ();
		tSafeLem<T>& operator = (tSafeLem<T> &copy);

	protected:
		bool requested;
		tMrSafety<T> *mParent;
		#ifdef GT_THREADS
			boost::unique_lock<boost::mutex> myLock;
		#endif

	friend class tMrSafety<T>;
	};

	//-------------------------------------------------------------------------------------
	//!\brief	Replicates the lemming spitter. Would have been nice to keep it as a base
	//!			class, but mr safety really needs to be its own thing.
	template<typename T>
	class tMrSafety{
	public:
		tMrSafety();
		virtual ~tMrSafety();

		void take(T* takeMe); 		//!< Cleans up the old data if it exists, and becomes the custodian of the data being past in. Waits to acquire lock.
		void cleanup();				//!< Deletes data. Waits to acquire lock.
		void drop(); 				//!< Don't manage this anymore. Doesn't cleanup. Waits to acquire lock.
		void set(const T& data);	//!< Set containing data and deletes any old data. Requires lock.
		tSafeLem<T> get();

	protected:
#ifdef GT_THREADS
		dThreadID current;
#endif

		int inTheWild;
		tSafeLem<T> *firstLem;

		#ifdef GT_THREADS
			boost::mutex dataMutex;
			boost::mutex wildLock;
			boost::condition_variable sync;
		#endif

		void deadLemming(tSafeLem<T>* corpse);	//!<	If this is the last lemming for this thread, release the lock for next thread in queue.
		void changedLem(const tSafeLem<T>* from, const tSafeLem<T>* to);	//!<
		T* getLockData(tSafeLem<T>* requester);	//!<	Acquires lock for current thread. Blocks and waits to acquire lock if request comes from another thread.
		bool isSameThread();

	private:
		T* mData;

	friend class tSafeLem<T>;
	};
}

///////////////////////////////////////////////////////////////////////////////////
// Templates
namespace gt{
//-------------------------------------------------------------------------------------
template<typename T>
tSafeLem<T>::tSafeLem(tMrSafety<T> *mr) :
	requested(false),
	mParent(mr)
{
}

template<typename T>
tSafeLem<T>::tSafeLem(tSafeLem<T> &lem) :
	requested(false),
	mParent(lem.mParent)
{
	mParent->changedLem(&lem, this);
	lem.mParent = NULL;
}

template<typename T>
tSafeLem<T>::~tSafeLem(){
	if(mParent!=NULL)
		mParent->deadLemming(this);
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
tSafeLem<T>::operator = (tSafeLem &copy){
	if(&copy != this){
		mParent = copy.mParent;
		mParent->changedLem(&copy, this);
		copy.mParent = NULL;
	}
	return *this;
}


//-------------------------------------------------------------------------------------

template<typename T>
tMrSafety<T>::tMrSafety() :
	inTheWild(0),
	mData(NULL)
{
	firstLem = NULL;
}

template<typename T>
tMrSafety<T>::~tMrSafety()
{
#ifdef GT_THREADS
	sync.notify_all();
	dataMutex.unlock();
	delete mData;
	wildLock.unlock();
#else
	delete mData;
#endif
}

template<typename T>
void
tMrSafety<T>::take(T* takeMe) {
	tSafeLem<T> internal(this);
	getLockData(&internal);
	mData = takeMe;
}

template<typename T>
void
tMrSafety<T>::cleanup() {
	tSafeLem<T> internal(this);
	getLockData(&internal);
	delete mData;
	mData = NULL;
}

template<typename T>
void
tMrSafety<T>::drop() {
	tSafeLem<T> internal(this);
	getLockData(&internal);
	mData = NULL;
}

template<typename T>
void
tMrSafety<T>::set(const T& data) {
	tSafeLem<T> internal(this);
	delete getLockData(&internal);
	mData = new T;
	*mData = data;
}

template<typename T>
tSafeLem<T>
tMrSafety<T>::get() {
	tSafeLem<T> temp(this);
	return temp;
	//return tSafeLem<T>(this);
}

template<typename T>
void
tMrSafety<T>::deadLemming(tSafeLem<T>* corpse){
	if(isMultithreading::yes()){
#ifdef GT_THREADS
		wildLock.lock();
		--inTheWild;
		if(inTheWild==0){
			firstLem = NULL;
			corpse->myLock.unlock();
			sync.notify_one();
		}
		wildLock. unlock();
#else
		DUMB_REF_ARG(corpse);
#endif
	}else{
		--inTheWild;
	}
}

template<typename T>
void
tMrSafety<T>::changedLem(const tSafeLem<T>* from, const tSafeLem<T>* to){
	DUMB_REF_ARG(from); DUMB_REF_ARG(to);
}

template<typename T>
T*
tMrSafety<T>::getLockData(tSafeLem<T>* requester){
	if(isMultithreading::yes()){
#ifdef GT_THREADS
		wildLock.lock();

		if(inTheWild==0){	//- There are no other requests waiting for this data.
			inTheWild = 1;
			firstLem = requester;
			current = boost::this_thread::get_id();
			wildLock.unlock();
			requester->myLock = dLock(dataMutex);
		}else if(!isSameThread()){	//- Another thread is using the data. When waken this thread becomes owner.
			wildLock.unlock();
			requester->myLock = dLock(dataMutex);
			sync.wait(requester->myLock);	//- Should block all requests from this thread.
			wildLock.lock();
			inTheWild = 1;
			firstLem = requester;
			current = boost::this_thread::get_id();
			wildLock.unlock();
		}else{
			if(!requester->requested)
				++inTheWild;
			wildLock.unlock();
		}
#endif
	}else{
		++inTheWild;
	}
	requester->requested = true;
	return mData;
}

template<typename T>
bool
tMrSafety<T>::isSameThread(){
#ifdef GT_THREADS
	if(isMultithreading::yes())
		return (inTheWild > 0 && current == boost::this_thread::get_id());

	return (inTheWild > 0);
#else
	return true;
#endif
}

}

#ifdef DEBUG
void threadTestFoo();
#endif


#endif
