/*
 * !\file	dirPtr.hpp
 * !\brief	Contains the directional pointer.
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

#ifndef FIGPTR_HPP
#define FIGPTR_HPP

#include "exceptions.hpp"
//#include <memory>

#ifdef GTUT
#	include "unitTests.hpp"
#endif

namespace gt{

	//-------------------------------------------------------------------------------------
	//!\brief	The content director is used to direct the smart
	//!			pointers to the content instance. This is different to just using boost
	//!			smart pointers because you are able to redirect every pointer to another
	//!			figment. When the count reaches 0, this content is cleaned up by the pointers.
	//!\note	Created because the boost smart pointers didn't provide a way to redirect all pointers to a new memory location.
	//!\todo	Prevent circular redirections.
	template<typename T>
	class tDirector{
	public:
		explicit tDirector(T* pContent);	//!< A director MUST begin life pointing to content. Otherwise, what are you doing??? Also, no using a local variable, which is why this is explicit.
		~tDirector();

		void redirect(tDirector<T>* pDirector);	//!< Causes this director to point to another instead. It will clean up its original content, and remain only able to point to other directors.
		void change(T* pNewContent);			//!< Change content.
		void link();	//!< Another pointer links in.
		void unlink();	//!< A pointer dies and unlinks. If this is the last link, the pointer must clean up the director.
		T* get() const;
		unsigned int getCount() const;

		T* mContent;				//!< The instance we are directed to. Or it can be null, in which case it must have a redirect.
		tDirector<T>* mRedirect;	//!< Redirection to another director, in which case count is ignored. If this isn't null, the content can't be as well.
		unsigned int mCount;		//!< Counts how many pointers are using this director.
	};

	//-------------------------------------------------------------------------------------
	//!\brief	Own brand of smart pointers which allow you to redirect all linked pointers
	//!			to another memory location.
	//!\note	Throws if you try to use it while it isn't linked.
	template<typename T>
	class tDirPtr{
	public:
		tDirPtr();	//!< Starts without a link.
		tDirPtr(T* pNewContent);	//!< Creates a new director and passes it the content.
		tDirPtr(tDirPtr<T> const &pPtr);	//!<
		~tDirPtr();		//!< Unlinks itself from the director.

		void redirect(tDirPtr<T> const &pFig);	//!< Also changes reference count used by smart pointers.
		void redirect(T* pNewContent);		//!< Allows you to redirect to a totally new memory location.
		T* get() const;				//!< Return pointer to the content. Please don't delete it.
		bool unique() const;		//!< Is this the last pointer?
		bool valid() const;			//!< not null?

		T* operator->() const;		//!< Alias for get().
		tDirPtr<T>& operator=(tDirPtr<T> const &pFDir);	//!< creates another link to the director.
		bool operator==(tDirPtr<T> const &pFDir) const;		//!< Compares memory address to see if two pointers are pointing at the same thing.
		bool operator!=(tDirPtr<T> const &pFDir) const;		//!< Same.
		T& operator*();		//!<

	protected:
		tDirector<T>* mDir;	//!< link to the director which contains the figment. Null until it is made equal to another pointer.
	};
}
////////////////////////////////////////////////////////////
namespace gt{
	template<typename T>
	tDirPtr<T>::tDirPtr(): mDir(NULL){}

	template<typename T>
	tDirPtr<T>::tDirPtr(T * pNewContent): mDir( new tDirector<T>(pNewContent) ){
		mDir->link();
	}

	template<typename T>
	tDirPtr<T>::tDirPtr(tDirPtr<T> const &pPtr): mDir(pPtr.mDir){
		if(mDir)
			mDir->link();
	}

	template<typename T>
	tDirPtr<T>::~tDirPtr(){
		try{
			if(mDir){
				if(unique())
					delete mDir;
				else
					mDir->unlink();
			}
		}catch(std::exception &e){
			excep::logExcep::add(e.what());

		}catch(...){
			excep::logExcep::add("Unknown error destroying tDirPtr");
		}
	}

	template<typename T>
	void
	tDirPtr<T>::redirect(tDirPtr<T> const &pFig){
		ASRT_NOTNULL(mDir);
		mDir->redirect(pFig.mDir);
	}

	template<typename T>
	void
	tDirPtr<T>::redirect(T* pNewContent){
		ASRT_NOTNULL(mDir);
		mDir->change(pNewContent);
	}

	template<typename T>
	T*
	tDirPtr<T>::get() const{
		ASRT_NOTNULL(mDir);
		return mDir->get();
	}

	template<typename T>
	bool
	tDirPtr<T>::unique() const{
		ASRT_NOTNULL(mDir);
		return mDir->getCount()<=1;
	}

	template<typename T>
	bool
	tDirPtr<T>::valid() const{
		return mDir != NULL;
	}

	template<typename T>
	T*
	tDirPtr<T>::operator->() const{
		ASRT_NOTNULL(mDir);
		return mDir->get();
	}

	template<typename T>
	tDirPtr<T>&
	tDirPtr<T>::operator=(tDirPtr<T> const &pFDir){
		if(!pFDir.mDir)
			return *this;

		if(mDir == pFDir.mDir)	// Should also handle self reference.
			return *this;

		if(mDir){
			if(unique())
				delete mDir;
			else
				mDir->unlink();
		}

		mDir = pFDir.mDir;
		mDir->link();

		return *this;
	}

	template<typename T>
	bool
	tDirPtr<T>::operator==(tDirPtr<T> const &pFDir) const{
		if(mDir==NULL){
			if(pFDir.mDir==NULL) return true; else return false;
		}else if(pFDir.mDir==NULL){
			return false;
		}
		return mDir->get() == pFDir.mDir->get();
	}

	template<typename T>
	bool
	tDirPtr<T>::operator!=(tDirPtr<T> const &pFDir) const{
		if(mDir==NULL){
			if(pFDir.mDir==NULL) return false; else return true;
		}else if(pFDir.mDir==NULL){
			return true;
		}
		return mDir->get() != pFDir.mDir->get();
	}

	template<typename T>
	T&
	tDirPtr<T>::operator*(){
		ASRT_NOTNULL(mDir);
		return *mDir->get();
	}
}


////////////////////////////////////////////////////////////
namespace gt{
	template<typename T>
	tDirector<T>::tDirector(T * pContent):
		mContent(pContent), mRedirect(NULL), mCount(0)
	{
	}

	template<typename T>
	tDirector<T>::~tDirector(){
		delete mContent;	// This should always be either valid, or NULL, which C++ delete can handle it.
		delete mRedirect;	// Same as content. Saves putting conditions here.
	}


	template<typename T>
	void
	tDirector<T>::redirect(tDirector<T>* pDirector){
		delete mContent;
		mContent=NULL;
		mRedirect = pDirector;
		mRedirect->mCount += mCount; //- Some more pointers joined the party.
		mCount = 0;	//- We no longer use this director for handling count. We defer to the other director.
	}

	template<typename T>
	void
	tDirector<T>::change(T* pNewContent){
		if(mRedirect){
			mRedirect->change(pNewContent);
		}else{
			delete mContent;
			mContent = pNewContent;
		}
	}

	template<typename T>
	void
	tDirector<T>::link(){
		if(mRedirect){
			mRedirect->link();
		}else{
			ASRT_NOTNULL(mContent);
			++mCount;
		}
	}

	template<typename T>
	void
	tDirector<T>::unlink(){
		if(mRedirect){
			mRedirect->unlink();
		}else{
			ASRT_NOTNULL(mContent);
			if(mCount < 1)
				throw excep::base_error("director underflow. Too many unlinks somehow...", __FILE__, __LINE__);

			--mCount;
		}
	}

	template<typename T>
	T*
	tDirector<T>::get() const{
		if(mRedirect){
			return mRedirect->get();
		}else{
			ASRT_NOTNULL(mContent);
			return mContent;
		}
	}

	template<typename T>
	unsigned int
	tDirector<T>::getCount() const{
		if(mRedirect){
			return mRedirect->getCount();
		}else{
			return mCount;
		}
	}

}
#endif
