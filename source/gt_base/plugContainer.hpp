/*
 * !\file	plugContainer.hpp
 * !\brief	If you need to serialize or pass about a container, you need a plug container.
 * !		Plug containers are plugs that use STL containers to hold other plugs. There's no
 * !		reason (that I can think of) to have a plug that uses an STL container for normal
 * !		data types.
 * !\note	Plug containers do NOT provide thread safety. That's up to the individual plugs they
 * !		contain.
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
*/

#ifndef PLUGCONTAINER_HPP
#define PLUGCONTAINER_HPP

#include "plug.hpp"


///////////////////////////////////////////////////////////////////////////////////
// Object types
namespace gt{

	//!\brief	Suitable for STL list and vector that contain cBase_plug child classes.
	template<typename CONTAINER>
	class tPlugLinierContainer : public cBase_plug{
	public:
		typedef typename CONTAINER::iterator itr_t;

		CONTAINER mContainer;

		tPlugLinierContainer();
		virtual ~tPlugLinierContainer();

		virtual void save(cByteBuffer* pSaveHere);
		virtual void loadEat(cByteBuffer* pChewToy, dReloadMap *aReloads = NULL);

		virtual	cBase_plug& operator= (const cBase_plug &pD);
		virtual bool operator== (const cBase_plug &pD) const;

#		ifdef GT_THREADS
			virtual void updateStart();
			virtual void updateFinish();
#		endif

		tPlugLinierContainer<CONTAINER> operator=(const tPlugLinierContainer<CONTAINER> &copyMe);

	protected:

		virtual void actualCopyInto(void* pContainer, dPlugType pType) const;
		virtual void actualCopyFrom(const void* pContainer, dPlugType pType);

#		ifdef GT_THREADS
			virtual void readShadow(cBase_plug *pWriteTo, dConSig aCon);
			virtual void writeShadow(const cBase_plug *pReadFrom, dConSig aCon);
#		endif
	};

	//!\brief	Implementation intended to make things easier that using tPlugLinierContainer
	template<typename T>
	class tPlugArray : public tPlugLinierContainer< std::vector< tPlug<T> > > {
	public:
		typedef typename std::vector< tPlug<T> > vec_t;

		tPlugArray(){}
		virtual ~tPlugArray(){}
	};

	//!\brief	Implementation intended to make things easier that using tPlugLinierContainer
	template<typename T>
	class tPlugList : public tPlugLinierContainer< std::vector< tPlug<T> > > {
	public:
		typedef typename std::list< tPlug<T> > list_t;

		tPlugList(){}
		virtual ~tPlugList(){}
	};


	//!\brief
	template<typename KEY, typename T>
	class tPlugMap : public cBase_plug {
	public:
		typedef typename std::map< KEY, tPlug<T> > map_t;
		typedef typename map_t::iterator itr_t;

		map_t mContainer;

		tPlugMap();
		virtual ~tPlugMap();

		virtual void save(cByteBuffer* pSaveHere);
		virtual void loadEat(cByteBuffer* pChewToy, dReloadMap *aReloads = NULL);

		virtual	cBase_plug& operator= (const cBase_plug &pD);
		virtual bool operator== (const cBase_plug &pD) const;

#		ifdef GT_THREADS
			virtual void updateStart();
			virtual void updateFinish();
#		endif

		tPlugMap<KEY, T> operator= (const tPlugMap<KEY, T> &copyMe);

	protected:
		typedef typename map_t::value_type entry;

		virtual void actualCopyInto(void* pContainer, dPlugType pType) const;
		virtual void actualCopyFrom(const void* pContainer, dPlugType pType);

#		ifdef GT_THREADS
			virtual void readShadow(cBase_plug *pWriteTo, dConSig aCon);
			virtual void writeShadow(const cBase_plug *pReadFrom, dConSig aCon);
#		endif
	};


	//!\brief	Allows you to contain any sort of plug in the same array. Which means you trade direct access to the data
	//!			in favour of flexibility.
	class tPlugJarArray : public tPlugArray< tPMorphJar<cBase_plug> > {
	public:
	};
}

///////////////////////////////////////////////////////////////////////////////////
// Templates
namespace gt{


	//-----------------------------------------------------------------------------------------------
	template<typename CONTAINER>
	tPlugLinierContainer<CONTAINER>::tPlugLinierContainer() :
		cBase_plug(cBase_plug::getPlugType<CONTAINER>())
	{}

	template<typename CONTAINER>
	tPlugLinierContainer<CONTAINER>::~tPlugLinierContainer(){
	}

	template<typename CONTAINER>
	void
	tPlugLinierContainer<CONTAINER>::save(cByteBuffer* pSaveHere){
		size_t s = mContainer.size();
		pSaveHere->add( &s );
		for(itr_t itr=mContainer.begin(); itr != mContainer.end(); ++itr)
			itr->save(pSaveHere);
	}

	template<typename CONTAINER>
	void
	tPlugLinierContainer<CONTAINER>::loadEat(cByteBuffer* pChewToy, dReloadMap *aReloads){
		size_t s=0;
		pChewToy->trimHead( pChewToy->fill(&s) );
		mContainer.insert(mContainer.begin(), s, 0);
		for(itr_t itr=mContainer.begin(); itr != mContainer.end(); ++itr)
			itr->loadEat(pChewToy, aReloads);
	}

	template<typename CONTAINER>
	cBase_plug&
	tPlugLinierContainer<CONTAINER>::operator= (const cBase_plug &pD){
		NOTSELF(&pD);
		pD.copyInto(&mContainer);
		return *this;
	}

	template<typename CONTAINER>
	bool
	tPlugLinierContainer<CONTAINER>::operator== (const cBase_plug &pD) const{
		return (mType == pD.mType);
	}

	template<typename CONTAINER>
	tPlugLinierContainer<CONTAINER>
	tPlugLinierContainer<CONTAINER>::operator=(const tPlugLinierContainer<CONTAINER> &copyMe){
		NOTSELF(&copyMe);
		mContainer = copyMe.mContainer;
		return *this;
	}

	template<typename CONTAINER>
	void
	tPlugLinierContainer<CONTAINER>::actualCopyInto(void* pContainer, dPlugType pType) const{
		if(pType != mType)
			throw excep::cantCopy(typeid(CONTAINER).name(), "unknown", __FILE__, __LINE__);

		*reinterpret_cast<CONTAINER*>(pContainer) = mContainer;
	}

	template<typename CONTAINER>
	void
	tPlugLinierContainer<CONTAINER>::actualCopyFrom(const void* pContainer, dPlugType pType){
		if(pType != mType)
			throw excep::cantCopy(typeid(CONTAINER).name(), "unknown", __FILE__, __LINE__);

		mContainer = *reinterpret_cast<const CONTAINER*>(pContainer);
	}

#	ifdef GT_THREADS

		template<typename CONTAINER>
		void
		tPlugLinierContainer<CONTAINER>::updateStart(){
			for(itr_t itr=mContainer.begin(); itr != mContainer.end(); ++itr)
				itr->updateStart();
		}

		template<typename CONTAINER>
		void
		tPlugLinierContainer<CONTAINER>::updateFinish(){
			for(itr_t itr=mContainer.begin(); itr != mContainer.end(); ++itr)
				itr->updateFinish();
		}

		template<typename CONTAINER>
		void
		tPlugLinierContainer<CONTAINER>::readShadow(cBase_plug *pWriteTo, dConSig aCon){
		}

		template<typename CONTAINER>
		void
		tPlugLinierContainer<CONTAINER>::writeShadow(const cBase_plug *pReadFrom, dConSig aCon){

		}

#	endif


	//-----------------------------------------------------------------------------------------------
	template<typename KEY, typename T>
	tPlugMap<KEY, T>::tPlugMap() :
		cBase_plug(cBase_plug::getPlugType<map_t>())
	{}

	template<typename KEY, typename T>
	tPlugMap<KEY, T>::~tPlugMap(){
	}

	template<typename KEY, typename T>
	void
	tPlugMap<KEY, T>::save(cByteBuffer* pSaveHere){
		size_t s = mContainer.size();
		pSaveHere->add( &s );
		for(itr_t itr=mContainer.begin(); itr!=mContainer.end(); ++itr){
			pSaveHere->add(&itr->first);
			itr->second.save(pSaveHere);
		}
	}

	template<typename KEY, typename T>
	void
	tPlugMap<KEY, T>::loadEat(cByteBuffer* pChewToy, dReloadMap *aReloads){
		size_t s=0;
		KEY tmpKey=0;
		pChewToy->trimHead( pChewToy->fill(&s) );
		for(size_t i=0; i < s; ++i){
			pChewToy->trimHead( pChewToy->fill(&tmpKey) );
			mContainer.insert(
				entry(tmpKey, tPlug<T>() )
			).first->second.loadEat(pChewToy);
		}
	}

	template<typename KEY, typename T>
	cBase_plug&
	tPlugMap<KEY, T>::operator= (const cBase_plug &pD){
		NOTSELF(&pD);
		pD.copyInto(&mContainer);
		return *this;
	}

	template<typename KEY, typename T>
	bool
	tPlugMap<KEY, T>::operator== (const cBase_plug &pD) const{
		return (mType == pD.mType);
	}

	template<typename KEY, typename T>
	tPlugMap<KEY, T>
	tPlugMap<KEY, T>::operator= (const tPlugMap<KEY, T> &copyMe){
		NOTSELF(&copyMe);

		mContainer = copyMe.mContainer;

		return *this;
	}

	template<typename KEY, typename T>
	void
	tPlugMap<KEY, T>::actualCopyInto(void* pContainer, dPlugType pType) const{
		if(pType != mType)
			throw excep::cantCopy(typeid(map_t).name(), "unknown", __FILE__, __LINE__);

		*reinterpret_cast<map_t*>(pContainer) = mContainer;
	}

	template<typename KEY, typename T>
	void
	tPlugMap<KEY, T>::actualCopyFrom(const void* pContainer, dPlugType pType){
		if(pType != mType)
			throw excep::cantCopy(typeid(map_t).name(), "unknown", __FILE__, __LINE__);

		mContainer = *reinterpret_cast<const map_t*>(pContainer);
	}

#	ifdef GT_THREADS

		template<typename KEY, typename T>
		void
		tPlugMap<KEY, T>::updateStart(){
			for(itr_t itr=mContainer.begin(); itr != mContainer.end(); ++itr)
				itr->second.updateStart();
		}

		template<typename KEY, typename T>
		void
		tPlugMap<KEY, T>::updateFinish(){
			for(itr_t itr=mContainer.begin(); itr != mContainer.end(); ++itr)
				itr->second.updateFinish();
		}

		template<typename KEY, typename T>
		void
		tPlugMap<KEY, T>::readShadow(cBase_plug *pWriteTo, dConSig aCon){
		}

		template<typename KEY, typename T>
		void
		tPlugMap<KEY, T>::writeShadow(const cBase_plug *pReadFrom, dConSig aCon){

		}

#	endif

}

///////////////////////////////////////////////////////////////////////////////////
// Template specializations.
namespace gt{

}


#endif
