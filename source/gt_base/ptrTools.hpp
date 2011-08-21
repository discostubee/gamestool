/*
 * !\file	ptrTools.hpp
 * !\brief	Contains a few pointer management classes.
 *
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
 *
 */

#ifndef PTRTOOLS_HPP
#define PTRTOOLS_HPP

#include "exceptions.hpp"
#include <memory>

#ifdef GTUT
#include "unitTests.hpp"
#endif

namespace gt{

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

	//-------------------------------------------------------------------------------------
	//!\brief
	template <typename T>
	class tPtrRef{
	public:
		tPtrRef();
		tPtrRef(T& ref);
		tPtrRef(T* ptr);
		tPtrRef(const tPtrRef<T>& other);
		~tPtrRef();

		T* operator -> ();
		tPtrRef<T>& operator = (const tPtrRef& other);

	private:
		T* myPtr;
	};

	//------------------------------------------------------------------------------------------
	//- Forward dec
	template<typename T> class tSpitLemming;

	//!\brief	Go forth and let me know when you die.
	//!\note	behaves like an auto pointer. Copying a lemming frees the old one.
	template<typename T>
	class tLemming{
	public:
		tLemming(tLemming<T>& copy);
		virtual ~tLemming();

		virtual T* operator -> ();
		virtual T* get();

		tLemming& operator = (const tLemming& copy);

	protected:
		tLemming();
		tLemming(tSpitLemming<T>* parent);

		void parentDied();

		tSpitLemming<T>* mParent;

	friend class tSpitLemming<T>;
	};

	//------------------------------------------------------------------------------------------
	//!\brief	Provides an interface for something you don't want to be handing out copies to.
	//!			This makes the class very useful for controlling a mutex lock.
	template<typename T>
	class tSpitLemming{
	public:
		tSpitLemming();
		virtual ~tSpitLemming();

		virtual tLemming<T>& get();
		virtual tSpitLemming<T>& operator = (const tSpitLemming<T>& spitter);

		unsigned int inWild();

	protected:
		virtual T* getData(const tLemming<T>* requester);
		virtual void deadLemming(const tLemming<T>* corpse);
		virtual void lemmingChange(const tLemming<T>* lem);

		//!\brief	called by lemming after its called the polymorphed deadLemming foo.
		void finish();

		unsigned int inTheWild;

	friend class tLemming<T>;
	};

}

////////////////////////////////////////////////////////////
namespace gt{
	template<typename T>
	tPtrRef<T>::tPtrRef() : myPtr(NULL) {}

	template<typename T>
	tPtrRef<T>::tPtrRef(T& ref) : myPtr(&ref) {}

	template<typename T>
	tPtrRef<T>::tPtrRef(T* ptr) : myPtr(ptr) {}

	template<typename T>
	tPtrRef<T>::tPtrRef(const tPtrRef<T>& other) : myPtr(other.myPtr) {}

	template<typename T>
	tPtrRef<T>::~tPtrRef() {}

	template<typename T>
	T*
	tPtrRef<T>::operator -> () { return myPtr; }

	template<typename T>
	tPtrRef<T>&
	tPtrRef<T>::operator = (const tPtrRef& other){
		if(this != &other) myPtr = other.myPtr; return *this;
	}
}

////////////////////////////////////////////////////////////
namespace gt{

	template<typename T>
	tLemming<T>::tLemming(tLemming<T>& copy) : mParent(copy.mParent){
		mParent->lemmingChange(this);
		copy.mParent = NULL;
	}

	template<typename T>
	tLemming<T>::tLemming(tSpitLemming<T>* parent) :
		mParent(parent)
	{
	}

	template<typename T>
	tLemming<T>::tLemming() :
		mParent(NULL)
	{}

	template<typename T>
	tLemming<T>::~tLemming(){
		if(mParent){
			mParent->deadLemming(this);
			mParent->finish();
		}

	}

	template<typename T> void
	tLemming<T>::parentDied(){
		mParent = false;
	}

	template<typename T> tLemming<T>&
	tLemming<T>::operator = (const tLemming& copy){
		if(this != &copy){
			mParent = copy.mParent;
			mParent->lemmingChange(this);
			copy.mParent = NULL;
		}
		return *this;
	}

	template<typename T> T*
	tLemming<T>:: operator -> (){
		return get();
	}

	template<typename T> T*
	tLemming<T>::get() {
		if(!mParent) throw std::exception();	// make this nicer.
		return mParent->getData(this);
	}
}

////////////////////////////////////////////////////////////
namespace gt{

	template<typename T>
	tSpitLemming<T>::tSpitLemming() : inTheWild(0) {

	}

	template<typename T>
	tSpitLemming<T>::~tSpitLemming() {}

	template<typename T> tLemming<T>&
	tSpitLemming<T>::get(){
		tLemming<T>* temp = new tLemming<T>(this);
		++inTheWild;

		return *temp;
	}

	template<typename T> tSpitLemming<T>&
	tSpitLemming<T>::operator = (const tSpitLemming<T>& spitter){
		return *this;
	}

	template<typename T> unsigned int
	tSpitLemming<T>::inWild() {
		return inTheWild;
	}

	template<typename T> T*
	tSpitLemming<T>::getData(const tLemming<T>* requester){
		return NULL;
	}

	template<typename T> void
	tSpitLemming<T>::deadLemming(const tLemming<T>* corpse) {
		DUMB_REF_ARG(corpse);
	}

	template<typename T> void
	tSpitLemming<T>::lemmingChange(const tLemming<T>* lem) {
		DUMB_REF_ARG(lem);
	}

	template<typename T> void
	tSpitLemming<T>::finish() {
		if (inTheWild==0) throw excep::base_error("lemming underflow", __FILE__, __LINE__);
		--inTheWild;
	}
}

#endif
