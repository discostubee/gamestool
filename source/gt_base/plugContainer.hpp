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
#include <vector>
#include <list>
#include <map>

///////////////////////////////////////////////////////////////////////////////////
// Object types
namespace gt{

	//----------------------------------------------------------------------------------------------------------------
	//!\brief
	template<
		typename PLUG_T,
		template<typename, typename> class CONT_T
	>
	class tPlugLinierContainer : public cBase_plug{
	public:

		//---
		virtual dPlugType getType() const;
		virtual void assignTo(void *pTo, dPlugType pType) const;
		virtual void appendTo(void *pTo, dPlugType pType) const;
		virtual void save(cByteBuffer* pSaveHere);
		virtual void loadEat(cByteBuffer* pChewToy, dReloadMap *aReloads = NULL);

		virtual bool operator== (const cBase_plug &pD) const;
		virtual	cBase_plug& operator= (const cBase_plug &pD);
		virtual cBase_plug& operator+= (const cBase_plug &pD);

#		ifdef GT_THREADS
			virtual void updateStart();
			virtual void updateFinish();
#		endif

		//--- new
		typedef CONT_T<
			tPlug<PLUG_T>,
			std::allocator<PLUG_T>
		> dContainer;	//!< The default template parameter doesn't appear to work when using it for template-template paramters.
		typedef tCoolItr<dContainer> itr_t;

		tPlugLinierContainer();
		virtual ~tPlugLinierContainer();

		tCoolItr<dContainer> getItr();

		template<typename OTHER> tPlugLinierContainer<PLUG_T, CONT_T>& operator= (const OTHER &pCopyMe);
		template<typename OTHER> tPlugLinierContainer<PLUG_T, CONT_T>& operator+= (const OTHER &pCopyMe);

	protected:

#		ifdef GT_THREADS
			virtual void readShadow(cBase_plug *pWriteTo, dConSig pCon);
			virtual void writeShadow(const cBase_plug *pReadFrom, dConSig pCon);
			virtual void shadowAppends(cBase_plug *pWriteTo, dConSig pSig);
			virtual void appendShadow(cBase_plug *pReadFrom, dConSig pSig);
#		endif

	private:
		dContainer mContainer;	//!<
//
//#		ifdef GT_THREADS
//			typename CONT_T< typename *tPlug<PLUG_T> > dShadows;
//
//			dShadows mShadows;
//#		endif
	};

}

///////////////////////////////////////////////////////////////////////////////////
// Templates
namespace gt{


	//-----------------------------------------------------------------------------------------------
	template< typename PLUG_T, template<typename, typename> class CONT_T >
	tPlugLinierContainer<PLUG_T, CONT_T>::tPlugLinierContainer(){
	}

	template< typename PLUG_T, template<typename, typename> class CONT_T >
	tPlugLinierContainer<PLUG_T, CONT_T>::~tPlugLinierContainer(){
	}

	template< typename PLUG_T, template<typename, typename> class CONT_T >
	cBase_plug::dPlugType
	tPlugLinierContainer<PLUG_T, CONT_T>::getType() const{
		return cBase_plug::genPlugType<dContainer>();
	}

	template< typename PLUG_T, template<typename, typename> class CONT_T >
	void
	tPlugLinierContainer<PLUG_T, CONT_T>::assignTo(void *aTo, dPlugType aType) const{
		if(aType != cBase_plug::genPlugType<dContainer>())
			throw excep::cantCopy("something", "linier container", __FILE__, __LINE__);

		*reinterpret_cast<dContainer*>(aTo) = mContainer;
	}

	template< typename PLUG_T, template<typename, typename> class CONT_T >
	void
	tPlugLinierContainer<PLUG_T, CONT_T>::appendTo(void *aTo, dPlugType aType) const{
		if(aType != cBase_plug::genPlugType<dContainer>())
			throw excep::cantCopy("something", "linier container", __FILE__, __LINE__);

		reinterpret_cast<dContainer*>(aTo)->insert(
			reinterpret_cast<dContainer*>(aTo)->end(),
			mContainer.begin(),
			mContainer.end()
		);
	}

	template< typename PLUG_T, template<typename, typename> class CONT_T >
	void
	tPlugLinierContainer<PLUG_T, CONT_T>::save(cByteBuffer* pSaveHere){
		size_t s = mContainer.size();
		pSaveHere->add( &s );
		for(typename dContainer::iterator itr = mContainer.begin(); itr != mContainer.end(); ++itr)
			itr->save(pSaveHere);
	}

	template< typename PLUG_T, template<typename, typename> class CONT_T >
	void
	tPlugLinierContainer<PLUG_T, CONT_T>::loadEat(cByteBuffer* pChewToy, dReloadMap *aReloads){
		typedef typename dContainer::value_type element;

		size_t s=0, i=0;
		pChewToy->trimHead( pChewToy->fill(&s) );
		while(i < s){
			mContainer.push_back(element());
			mContainer.back().loadEat(pChewToy, aReloads);
			++i;
		}
	}

	template< typename PLUG_T, template<typename, typename> class CONT_T >
	bool
	tPlugLinierContainer<PLUG_T, CONT_T>::operator== (const cBase_plug &pD) const{
		return (getType() == pD.getType());
	}

	template< typename PLUG_T, template<typename, typename> class CONT_T >
	cBase_plug&
	tPlugLinierContainer<PLUG_T, CONT_T>::operator= (const cBase_plug &pD){
		NOTSELF(&pD);

		if(pD.getType() == genPlugType<dContainer>()){
			mContainer = dynamic_cast<
				const tPlugLinierContainer<PLUG_T, CONT_T>*
			>(&pD)->mContainer;
		}else{
			mContainer.clear();	//- ensure we start fresh.
			mContainer.resize(1);
			*mContainer.begin() = pD;
		}

		return *this;
	}

	template< typename PLUG_T, template<typename, typename> class CONT_T >
	cBase_plug&
	tPlugLinierContainer<PLUG_T, CONT_T>::operator+= (const cBase_plug &pD){
		NOTSELF(&pD);

		if(pD.getType() == genPlugType<dContainer>()){
			const dContainer &tmp = dynamic_cast<
				const tPlugLinierContainer<PLUG_T, CONT_T>*
			>(&pD)->mContainer;

			mContainer.insert(
				mContainer.end(),
				tmp.begin(),
				tmp.end()
			);
		}else{
			try{
				*mContainer.insert(mContainer.end(), tPlug<PLUG_T>()) = pD;
			}catch(...){
				//mContainer.erase(mContainer.begin() + (mContainer.size()-1));
				throw;
			}
		}

		return *this;
	}

	template< typename PLUG_T, template<typename, typename> class CONT_T >
	tCoolItr< typename tPlugLinierContainer<PLUG_T, CONT_T>::dContainer >
	tPlugLinierContainer<PLUG_T, CONT_T>::getItr(){
		return tCoolItr<dContainer>(&mContainer);
	}

	template< typename PLUG_T, template<typename, typename> class CONT_T >
	template<typename OTHER>
	tPlugLinierContainer<PLUG_T, CONT_T>&
	tPlugLinierContainer<PLUG_T, CONT_T>::operator= (const OTHER &pCopyMe){
		return *this;
	}

	template< typename PLUG_T, template<typename, typename> class CONT_T >
	template<typename OTHER>
	tPlugLinierContainer<PLUG_T, CONT_T>&
	tPlugLinierContainer<PLUG_T, CONT_T>::operator+= (const OTHER &pCopyMe){
		return *this;
	}

#	ifdef GT_THREADS

		template< typename PLUG_T, template<typename, typename> class CONT_T >
		void
		tPlugLinierContainer<PLUG_T, CONT_T>::updateStart(){
			for(fullitr_t itr=mContainer.begin(); itr != mContainer.end(); ++itr)
				itr->updateStart();
		}

		template< typename PLUG_T, template<typename, typename> class CONT_T >
		void
		tPlugLinierContainer<PLUG_T, CONT_T>::updateFinish(){
			for(fullitr_t itr=mContainer.begin(); itr != mContainer.end(); ++itr)
				itr->updateFinish();
		}
//
//		template< typename PLUG_T, template<typename, typename> class CONT_T >
//		void
//		tPlugLinierContainer<PLUG_T, CONT_T>::readShadow(cBase_plug *pWriteTo, dConSig aCon){
//		}
//
//		template< typename PLUG_T, template<typename, typename> class CONT_T >
//		void
//		tPlugLinierContainer<PLUG_T, CONT_T>::writeShadow(const cBase_plug *pReadFrom, dConSig aCon){
//
//		}
//
//		template< typename PLUG_T, template<typename, typename> class CONT_T >
//		void
//		shadowAppends(cBase_plug *pWriteTo, dConSig pSig){
//		}
//
//		template< typename PLUG_T, template<typename, typename> class CONT_T >
//		void
//		appendShadow(cBase_plug *pReadFrom, dConSig pSig){
//		}

#	endif


//	//-----------------------------------------------------------------------------------------------
//	template<typename KEY, typename T>
//	tPlugMap<KEY, T>::tPlugMap(){
//	}
//
//	template<typename KEY, typename T>
//	tPlugMap<KEY, T>::~tPlugMap(){
//	}
//
//	template<typename KEY, typename T>
//	cBase_plug::dPlugType
//	tPlugMap<KEY, T>::getType() const{
//		return cBase_plug::genPlugType<map_t>();
//	}
//
//	template<typename KEY, typename T>
//	void
//	tPlugMap<KEY, T>::save(cByteBuffer* pSaveHere){
//		size_t s = mContainer.size();
//		pSaveHere->add( &s );
//		for(itr_t itr=mContainer.begin(); itr!=mContainer.end(); ++itr){
//			pSaveHere->add(&itr->first);
//			itr->second.save(pSaveHere);
//		}
//	}
//
//	template<typename KEY, typename T>
//	void
//	tPlugMap<KEY, T>::loadEat(cByteBuffer* pChewToy, dReloadMap *aReloads){
//		size_t s=0;
//		KEY tmpKey=0;
//		pChewToy->trimHead( pChewToy->fill(&s) );
//		for(size_t i=0; i < s; ++i){
//			pChewToy->trimHead( pChewToy->fill(&tmpKey) );
//			mContainer.insert(
//				entry(tmpKey, tPlug<T>() )
//			).first->second.loadEat(pChewToy);
//		}
//	}
//
//	template<typename KEY, typename T>
//	void
//	tPlugMap<KEY, T>::assign(void *aTo, dPlugType aType) const{
//		if(aType != cBase_plug::genPlugType<map_t>())
//			throw excep::cantCopy("something", "plug map", __FILE__, __LINE__);
//
//		*reinterpret_cast<map_t*>(aTo) = mContainer;
//	}
//
//	template<typename KEY, typename T>
//	void
//	tPlugMap<KEY, T>::append(void *aTo, dPlugType aType) const{
//		if(aType != cBase_plug::genPlugType<map_t>())
//			throw excep::cantCopy("append something", "plug map", __FILE__, __LINE__);
//
//		reinterpret_cast<map_t*>(aTo)->insert(
//			mContainer.begin(), mContainer.end()
//		);
//	}
//
//	template<typename KEY, typename T>
//	cBase_plug&
//	tPlugMap<KEY, T>::operator= (const cBase_plug &pD){
//		NOTSELF(&pD);
//		pD.assign(
//			&mContainer,
//			cBase_plug::genPlugType< tPlugMap<KEY, T> >()
//		);
//		return *this;
//	}
//
//	template<typename KEY, typename T>
//	bool
//	tPlugMap<KEY, T>::operator== (const cBase_plug &pD) const{
//		return (getType() == pD.getType());
//	}
//
//	template<typename KEY, typename T>
//	cBase_plug&
//	tPlugMap<KEY, T>::operator+= (const cBase_plug &pD){
//		NOTSELF(&pD);
//		map_t tmp;
//		pD.assign(&tmp, getType());
//		for(
//			typename map_t::iterator itr = tmp.begin();
//			itr != tmp.end();
//			++itr
//		){
//			mContainer[ itr->first ] = itr->second;
//		}
//		return *this;
//	}
//
//	template<typename KEY, typename T>
//	tPlugMap<KEY, T>
//	tPlugMap<KEY, T>::operator= (const tPlugMap<KEY, T> &copyMe){
//		NOTSELF(&copyMe);
//
//		mContainer = copyMe.mContainer;
//
//		return *this;
//	}
//
//
//#	ifdef GT_THREADS
//
//		template<typename KEY, typename T>
//		void
//		tPlugMap<KEY, T>::updateStart(){
//			for(itr_t itr=mContainer.begin(); itr != mContainer.end(); ++itr)
//				itr->second.updateStart();
//		}
//
//		template<typename KEY, typename T>
//		void
//		tPlugMap<KEY, T>::updateFinish(){
//			for(itr_t itr=mContainer.begin(); itr != mContainer.end(); ++itr)
//				itr->second.updateFinish();
//		}
//
//		template<typename KEY, typename T>
//		void
//		tPlugMap<KEY, T>::readShadow(cBase_plug *pWriteTo, dConSig aCon){
//		}
//
//		template<typename KEY, typename T>
//		void
//		tPlugMap<KEY, T>::writeShadow(const cBase_plug *pReadFrom, dConSig aCon){
//		}
//
//		template<typename KEY, typename T>
//		void
//		tPlugMap<KEY, T>::appendShadow(const cBase_plug *pReadFrom, dConSig aCon){
//
//		}
//#	endif

}

///////////////////////////////////////////////////////////////////////////////////
// Template specializations.
namespace gt{

}


#endif
