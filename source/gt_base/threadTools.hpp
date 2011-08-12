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

#include "ptrTools.hpp"

namespace gt{

	template<typename T>
	class tMrSafety: public tSpitLemming<T>{
	public:
		tMrSafety();
		~tMrSafety();

		void take(T* takeMe);	//!< Cleans up the old data if it exists, and becomes the custodian of the data being past in.
		void cleanup();			//!< Deletes data.
		void drop();			//!< Don't manage this anymore. Doesn't cleanup.

	protected:
		virtual T* getData();
		virtual void deadLemming(const tLemming<T>* corpse);

		tMrSafety<T>& operator = (T* takeMe);

	private:
		T* mData;
	};
}

////////////////////////////////////////////////////////////
namespace gt{
	template<typename T>
	tMrSafety<T>::tMrSafety() : mData(NULL) {}

	template<typename T>
	tMrSafety<T>::~tMrSafety() { delete mData; }	//- Should be able to handle null pointer.

	template<typename T> void
	tMrSafety<T>::take(T* takeMe){
		if(mData) delete mData;

		mData = takeMe;
	}

	template<typename T> void
	tMrSafety<T>::cleanup(){
		delete mData; mData = NULL;
	}

	template<typename T> void
	tMrSafety<T>::drop(){
		mData = NULL;
	}

	template<typename T> T*
	tMrSafety<T>::getData(){
		//!\todo Acquire lock
		return mData;
	}

	template<typename T> void
	tMrSafety<T>::deadLemming(const tLemming<T>* corpse){
		//!\todo release lock.
	}

	template<typename T> tMrSafety<T>&
	tMrSafety<T>::operator = (T* takeMe){
		return *this;
	}
}

#endif
