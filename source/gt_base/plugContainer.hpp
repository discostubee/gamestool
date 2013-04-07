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

	//----------------------------------------------------------------------------------------------------------------
	//!\brief	Suitable for STL list and vector that contain cBase_plug child classes.
	template<typename CONTAINER>
	class tPlugLinierContainer : public cBase_plug{
	public:
		typedef tCoolItr<CONTAINER> itr_t;

		CONTAINER mContainer;	//!< Exposed to make things easy.

		tPlugLinierContainer();
		virtual ~tPlugLinierContainer();

		virtual dPlugType getType() const;
		virtual void save(cByteBuffer* pSaveHere);
		virtual void loadEat(cByteBuffer* pChewToy, dReloadMap *aReloads = NULL);
		virtual void assign(void *aTo, dPlugType aType) const;
		virtual void append(void *aTo, dPlugType aType) const;

		virtual	cBase_plug& operator= (const cBase_plug &pD);
		virtual bool operator== (const cBase_plug &pD) const;
		virtual cBase_plug& operator+= (const cBase_plug &pD);

		//--- new
		virtual	cBase_plug& operator = (const CONTAINER &pCopyMe);
		tPlugLinierContainer<CONTAINER>& operator= (const tPlugLinierContainer<CONTAINER> &pCopyMe);
		template<typename OTHER> tPlugLinierContainer<CONTAINER>& operator+= (const OTHER &pCopyMe);

		itr_t getItr();

#		ifdef GT_THREADS
			virtual void updateStart();
			virtual void updateFinish();
#		endif

	protected:
		typedef typename CONTAINER::iterator fullitr_t;	//!< Container iterator.

#		ifdef GT_THREADS
			virtual void readShadow(cBase_plug *pWriteTo, dConSig aCon);
			virtual void writeShadow(const cBase_plug *pReadFrom, dConSig aCon);
			virtual void appendShadow(const cBase_plug *pReadFrom, dConSig aCon);
#		endif
	};

	//----------------------------------------------------------------------------------------------------------------
	//!\brief	Implementation intended to make things easier that using tPlugLinierContainer
	template<typename T>
	class tPlugArray : public tPlugLinierContainer< std::vector< tPlug<T> > > {
	public:
		typedef typename std::vector< tPlug<T> > vec_t;

		tPlugArray(){}
		virtual ~tPlugArray(){}
	};

	//----------------------------------------------------------------------------------------------------------------
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

		dPlugType getType() const;
		virtual void save(cByteBuffer* pSaveHere);
		virtual void loadEat(cByteBuffer* pChewToy, dReloadMap *aReloads = NULL);
		virtual void assign(void *aTo, dPlugType aType) const;
		virtual void append(void *aTo, dPlugType aType) const;

		virtual	cBase_plug& operator= (const cBase_plug &pD);
		virtual bool operator== (const cBase_plug &pD) const;
		virtual	cBase_plug& operator+= (const cBase_plug &pD);

#		ifdef GT_THREADS
			virtual void updateStart();
			virtual void updateFinish();
#		endif

		//--- new
		tPlugMap<KEY, T> operator= (const tPlugMap<KEY, T> &copyMe);

	protected:
		typedef typename map_t::value_type entry;

#		ifdef GT_THREADS
			virtual void readShadow(cBase_plug *pWriteTo, dConSig aCon);
			virtual void writeShadow(const cBase_plug *pReadFrom, dConSig aCon);
			virtual void appendShadow(const cBase_plug *pReadFrom, dConSig aCon);
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
	tPlugLinierContainer<CONTAINER>::tPlugLinierContainer(){
	}

	template<typename CONTAINER>
	tPlugLinierContainer<CONTAINER>::~tPlugLinierContainer(){
	}

	template<typename CONTAINER>
	void
	tPlugLinierContainer<CONTAINER>::save(cByteBuffer* pSaveHere){
		size_t s = mContainer.size();
		pSaveHere->add( &s );
		for(fullitr_t itr=mContainer.begin(); itr != mContainer.end(); ++itr)
			itr->save(pSaveHere);
	}

	template<typename CONTAINER>
	void
	tPlugLinierContainer<CONTAINER>::loadEat(cByteBuffer* pChewToy, dReloadMap *aReloads){
		typedef typename CONTAINER::value_type element;

		size_t s=0, i=0;
		pChewToy->trimHead( pChewToy->fill(&s) );
		mContainer.reserve(s);
		while(i < s){
			mContainer.push_back(element());
			mContainer.back().loadEat(pChewToy, aReloads);
			++i;
		}
	}

	template<typename CONTAINER>
	void
	tPlugLinierContainer<CONTAINER>::assign(void *aTo, dPlugType aType) const{
	}

	template<typename CONTAINER>
	void
	tPlugLinierContainer<CONTAINER>::append(void *aTo, dPlugType aType) const{

	}

	template<typename CONTAINER>
	cBase_plug&
	tPlugLinierContainer<CONTAINER>::operator= (const cBase_plug &pD){
		NOTSELF(&pD);
		pD.assign(&mContainer, getType());
		return *this;
	}

	template<typename CONTAINER>
	bool
	tPlugLinierContainer<CONTAINER>::operator== (const cBase_plug &pD) const{
		return (getType() == pD.getType());
	}

	template<typename CONTAINER>
	cBase_plug&
	tPlugLinierContainer<CONTAINER>::operator+= (const cBase_plug &pD){
		NOTSELF(&pD);
		CONTAINER tmp;
		pD.assign(&tmp, getType());
		mContainer.reserve(mContainer.size() + tmp.size());
		for(
			typename CONTAINER::const_iterator itr = tmp.begin();
			itr != tmp.end();
			++itr
		)
			mContainer.push_back(*itr);

		return *this;
	}


	template<typename CONTAINER>
	cBase_plug&
	tPlugLinierContainer<CONTAINER>::operator= (const CONTAINER &pCopyMe){
		mContainer = pCopyMe;
		return *this;
	}

	template<typename CONTAINER>
	tPlugLinierContainer<CONTAINER>&
	tPlugLinierContainer<CONTAINER>::operator=(const tPlugLinierContainer<CONTAINER> &copyMe){
		NOTSELF(&copyMe);
		mContainer = copyMe.mContainer;
		return *this;
	}

	template<typename CONTAINER>
	template<typename OTHER>
	tPlugLinierContainer<CONTAINER>&
	tPlugLinierContainer<CONTAINER>::operator+=(const OTHER &pCopyMe){
		mContainer.reserve(mContainer.size() + pCopyMe.size());
		for(
			typename OTHER::const_iterator itr = pCopyMe.begin();
			itr != pCopyMe.end();
			++itr
		)
			mContainer.push_back(*itr);

		return *this;
	}

	template<typename CONTAINER>
	tCoolItr<CONTAINER>
	tPlugLinierContainer<CONTAINER>::getItr(){
		return tCoolItr<CONTAINER>(&mContainer);
	}


#	ifdef GT_THREADS

		template<typename CONTAINER>
		void
		tPlugLinierContainer<CONTAINER>::updateStart(){
			for(fullitr_t itr=mContainer.begin(); itr != mContainer.end(); ++itr)
				itr->updateStart();
		}

		template<typename CONTAINER>
		void
		tPlugLinierContainer<CONTAINER>::updateFinish(){
			for(fullitr_t itr=mContainer.begin(); itr != mContainer.end(); ++itr)
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

		template<typename CONTAINER>
		void
		tPlugLinierContainer<CONTAINER>::appendShadow(const cBase_plug *pReadFrom, dConSig aCon){

		}

#	endif


	//-----------------------------------------------------------------------------------------------
	template<typename KEY, typename T>
	tPlugMap<KEY, T>::tPlugMap(){
	}

	template<typename KEY, typename T>
	tPlugMap<KEY, T>::~tPlugMap(){
	}

	template<typename KEY, typename T>
	cBase_plug::dPlugType
	tPlugMap<KEY, T>::getType() const{
		return cBase_plug::genPlugType<map_t>();
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
	void
	tPlugMap<KEY, T>::assign(void *aTo, dPlugType aType) const{

	}

	template<typename KEY, typename T>
	void
	tPlugMap<KEY, T>::append(void *aTo, dPlugType aType) const{

	}

	template<typename KEY, typename T>
	cBase_plug&
	tPlugMap<KEY, T>::operator= (const cBase_plug &pD){
		NOTSELF(&pD);
		pD.assign(
			&mContainer,
			cBase_plug::genPlugType< tPlugMap<KEY, T> >()
		);
		return *this;
	}

	template<typename KEY, typename T>
	bool
	tPlugMap<KEY, T>::operator== (const cBase_plug &pD) const{
		return (getType() == pD.getType());
	}

	template<typename KEY, typename T>
	cBase_plug&
	tPlugMap<KEY, T>::operator+= (const cBase_plug &pD){
		NOTSELF(&pD);
		map_t tmp;
		pD.assign(&tmp, getType());
		for(
			typename map_t::iterator itr = tmp.begin();
			itr != tmp.end();
			++itr
		){
			mContainer[ itr->first ] = itr->second;
		}
		return *this;
	}

	template<typename KEY, typename T>
	tPlugMap<KEY, T>
	tPlugMap<KEY, T>::operator= (const tPlugMap<KEY, T> &copyMe){
		NOTSELF(&copyMe);

		mContainer = copyMe.mContainer;

		return *this;
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

		template<typename KEY, typename T>
		void
		tPlugMap<KEY, T>::appendShadow(const cBase_plug *pReadFrom, dConSig aCon){

		}
#	endif

}

///////////////////////////////////////////////////////////////////////////////////
// Template specializations.
namespace gt{

}


#endif
