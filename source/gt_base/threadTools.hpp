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
	//!\brief
	template<typename T>
	class tMrSafety : public tSpitLemming<T>{
	public:
		typedef typename tSpitLemming<T>::tLemming dLemming;

		tMrSafety();
		~tMrSafety();
		dLemming get();
		void take(T *takeMe); 		//!< Cleans up the old data if it exists, and becomes the custodian of the data being past in. Waits to acquire lock.
		void cleanup();				//!< Deletes data. Waits to acquire lock.
		void drop(); 				//!< Don't manage this anymore. Doesn't cleanup. Waits to acquire lock.
		void set(const T& data);	//!< Set containing data and deletes any old data. Requires lock.

	protected:
		void changeLem(dLemming *from, dLemming *to);	//!< Makes the 'from' lemming empty and makes the 'to' lemming linked to this manager.
		void first();	//!< Starts the lock.
		void noMore();	//!< If this is the last lemming for this thread, release the lock for next thread in queue.
		T* getData(const dLemming *requester);	//!< Acquires lock for current thread. Blocks and waits to acquire lock if request comes from another thread.

	private:
		T* mData;

#		ifdef GT_THREADS
			boost::recursive_mutex muData;
			typedef boost::lock_guard<boost::recursive_mutex> dMuLock;
#		endif
	};

}

///////////////////////////////////////////////////////////////////////////////////
// Templates
namespace gt{


//-------------------------------------------------------------------------------------

template<typename T>
tMrSafety<T>::tMrSafety()
: mData(NULL)
{}

template<typename T>
tMrSafety<T>::~tMrSafety(){
	cleanup();
}

template<typename T>
typename tMrSafety<T>::dLemming
tMrSafety<T>::get() {
#	ifdef GT_THREADS
		dMuLock lock(muData);
#	endif

	return tSpitLemming<T>::get();
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
void
tMrSafety<T>::first(){
#	ifdef GT_THREADS
		muData.lock();
#	endif
}

template<typename T>
void
tMrSafety<T>::noMore(){
#	ifdef GT_THREADS
		muData.unlock();
#	endif
}

template<typename T>
T*
tMrSafety<T>::getData(const dLemming *requester){
	return mData;
}

template<typename T>
void
tMrSafety<T>::changeLem(dLemming *from, dLemming *to){
#	ifdef GT_THREADS
		dMuLock lock(muData);
#	endif
	tSpitLemming<T>::changeLem(from, to);
}

}

#endif
