/*
 * !\file	ptrTools.hpp
 * !\brief	Contains a few pointer management classes.
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
 *
 */

#ifndef PTRTOOLS_HPP
#define PTRTOOLS_HPP

#include "utils.hpp"
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
	//!\brief	Used as a non-smart pointer wrapper
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
	//!\brief
	template<typename T>
	class tSpitLemming{
	public:

		//!\brief	Let me know when you die.
		//!\note	behaves like an auto pointer, copying a lemming frees the old one.
		class tLemming{
		public:
			tLemming(const tLemming& copy);
			~tLemming();

			T* operator -> ();
			T* get();

			tLemming& operator = (const tLemming& copy);

		protected:
			tLemming();
			tLemming(tSpitLemming<T>* parent);

			tSpitLemming<T>* mParent;

		friend class tSpitLemming<T>;
		};

		tSpitLemming();
		virtual ~tSpitLemming();

		virtual tLemming get();
		virtual tSpitLemming<T>& operator= (const tSpitLemming<T>& spitter);

	protected:
		unsigned int inTheWild;

		virtual T* getData(const tLemming* requester) =0;
		virtual void first() =0;	//!< Called when the first lemming is made.
		virtual void noMore() =0;	//!< Called when the last lemming dies

		virtual void changeLem(tLemming* from, tLemming* to);	//!< Makes the 'from' lemming empty and makes the 'to' lemming linked to this manager.

		void finish();	//!<

	friend class tLemming;
	};

	//------------------------------------------------------------------------------------------
	//!\brief	Polymorph jar gives you the ability to pass about a managed or unmanaged pointer,
	//!			without forcing the user to know which of the two it is.
	template<typename T>
	class tPMorphJar{
	private:
		typedef T* (*dCopyAlloc)(const void*);

		T *data;
		bool castodian;
		dCopyAlloc fPtrCopAll;

		template<typename REAL> static T* copyAlloc(const void *copyMe){
			T *tmp = new REAL();
			if(copyMe)
				*reinterpret_cast<REAL*>(tmp) = *reinterpret_cast<const REAL*>(copyMe);
			else
				memset(tmp, 0, sizeof(REAL));
			return tmp;
		}

	public:
		explicit tPMorphJar() :
			data(NULL), castodian(false), fPtrCopAll(NULL)
		{}

		explicit tPMorphJar(const tPMorphJar<T> &copyMe):
			castodian(copyMe.castodian), fPtrCopAll(copyMe.fPtrCopAll)
		{
			if(fPtrCopAll){
				data = fPtrCopAll(copyMe.data);
				castodian = true;

			}else{
				data = copyMe.data;
				castodian = false;
			}
		}

		template<typename COPY> tPMorphJar(const tPMorphJar<COPY> &copyMe):
			fPtrCopAll(copyMe.fPtrCopAll)
		{
			if(fPtrCopAll){
				data = fPtrCopAll(copyMe.data);
				castodian = true;

			}else{
				data = copyMe.data;
				castodian = false;
			}
		}

		template<typename COPY> tPMorphJar(const COPY &copyMe) :
			castodian(true), fPtrCopAll(copyAlloc<COPY>)
		{
			data = fPtrCopAll(&copyMe);
		}

		template<typename COPY> tPMorphJar(COPY *refMe) :
			castodian(false), fPtrCopAll(NULL)
		{
			data = refMe;
		}

		~tPMorphJar(){
			if(castodian)
				delete data;
		}

		tPMorphJar<T> & operator = (const tPMorphJar<T> &otherJar){
			NOTSELF(&otherJar);

			if(castodian)
				delete data;

			fPtrCopAll = otherJar.fPtrCopAll;

			if(fPtrCopAll)
				data = fPtrCopAll(otherJar.data);
			else
				data = NULL;

			return *this;
		}

		//!\brief	Create memory and Copy the target's values.
		template<typename COPY> tPMorphJar<T> & operator = (const COPY &copyMe){
			if(castodian)
				delete data;

			fPtrCopAll = copyAlloc<COPY>;
			data = fPtrCopAll(&copyMe);
			castodian = true;
			return *this;
		}

		//!\brief	The jar is simply just a reference to some memory. BE CAREFUL not to delete the memory
		//!			before this jar is either deleted or given memory it can manage.
		template<typename REF> tPMorphJar<T> & operator = (REF *refMe){
			if(castodian)
				delete data;

			fPtrCopAll = NULL;

			data = refMe;
			castodian = false;
			return *this;
		}

		//!\brief	Allows you access to the content directly. BE CAREFUL, it's not passing back a copy.
		T& get(){
			return *data;
		}
	};

	//------------------------------------------------------------------------------------------
	//!\brief	Handy as the boost lib uses copy operator on pointer.
	template<typename TYPE>
	class tAutoPtr{
	public:
		tAutoPtr(const tAutoPtr<TYPE> &pTransfer) {
			mManage = const_cast< tAutoPtr<TYPE> &>(pTransfer).mManage;
			const_cast< tAutoPtr<TYPE> &>(pTransfer).mManage = NULL;
		}

		tAutoPtr(TYPE *pManageMe) : mManage(pManageMe) {}
		~tAutoPtr() { delete mManage; }

		TYPE* operator*() { return mManage; }
		TYPE* operator->() { return mManage; }

	private:
		TYPE *mManage;
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
	tSpitLemming<T>::tLemming::tLemming(const tLemming& copy)
	: mParent(copy.mParent)
	{
		mParent->changeLem(const_cast<tLemming*>(&copy), this);
	}

	template<typename T>
	tSpitLemming<T>::tLemming::tLemming(tSpitLemming<T>* parent)
	: mParent(parent)
	{}

	template<typename T>
	tSpitLemming<T>::tLemming::tLemming()
	: mParent(NULL)
	{}

	template<typename T>
	tSpitLemming<T>::tLemming::~tLemming(){
		if(mParent)
			mParent->finish();
	}

	template<typename T>
	typename tSpitLemming<T>::tLemming&
	tSpitLemming<T>::tLemming::operator= (const tLemming& copy){
		if(this != &copy){
			mParent = copy.mParent;
			mParent->changeLem(const_cast<tLemming*>(&copy), this);
		}
		return *this;
	}

	template<typename T>
	T*
	tSpitLemming<T>::tLemming::operator-> (){
		return get();
	}

	template<typename T>
	T*
	tSpitLemming<T>::tLemming::get() {
		if(!mParent)
			throw excep::base_error("bad parent", __FILE__, __LINE__);

		return mParent->getData(this);
	}


	//------------------------------------------------
	template<typename T>
	tSpitLemming<T>::tSpitLemming()
	: inTheWild(0)
	{}

	template<typename T>
	tSpitLemming<T>::~tSpitLemming() {
		if(inTheWild != 0)
			SHITPANTS;
	}

	template<typename T>
	typename tSpitLemming<T>::tLemming
	tSpitLemming<T>::get(){
		if(inTheWild==0)
			first();

		++inTheWild;
		return tLemming(this);
	}

	template<typename T>
	tSpitLemming<T>&
	tSpitLemming<T>::operator= (const tSpitLemming<T>& spitter){
		return *this;
	}

	template<typename T>
	void
	tSpitLemming<T>::changeLem(typename tSpitLemming<T>::tLemming *from, typename tSpitLemming<T>::tLemming *to){
		ASRT_TRUE(from->mParent == this, "Tried to change from a lemming that didn't come from this manager.");

		to->mParent = this;
		from->mParent = NULL;
	}

	template<typename T> void
	tSpitLemming<T>::finish() {
		if (inTheWild==0)
			throw excep::base_error("lemming underflow", __FILE__, __LINE__);

		--inTheWild;
		if(inTheWild==0)
			noMore();
	}


}

#endif
