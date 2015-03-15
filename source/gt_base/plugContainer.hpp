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
#include "plugContainerGetter.hpp"

#include <vector>
#include <list>
#include <map>

///////////////////////////////////////////////////////////////////////////////////
// Objects.
namespace gt{

	//----------------------------------------------------------------------------------------------------------------
	//!\brief	Works with things like vectors and linked lists which don't have any special uses like a map does.
	template<
		typename ELEM_T,
		template<typename, typename> class CONT_T
	>
	class tPlugLinearContainer : public cBase_plugContainer{
	public:

		//--- override container
		dPlugType getType() const;
		size_t getCount() const;
		cBase_plug* getPlug(size_t idx);
		const cBase_plug* getPlugConst(size_t idx) const;
		void add(const cBase_plug &addMe);
		void clear();

		//--- override base plug
		void assignTo(void *pTo, dPlugType pType) const;
		void appendTo(void *pTo, dPlugType pType) const;
		void save(cByteBuffer* pSaveHere);
		void loadEat(cByteBuffer* pChewToy, dReloadMap *aReloads = NULL);

		bool operator== (const cBase_plug &pCompare) const;
		cBase_plug& operator= (const cBase_plug &pCopyMe);
		cBase_plug& operator+= (const cBase_plug &pCopyMe);

#		ifdef GT_THREADS
			virtual void updateStart();
			virtual void updateFinish();
#		endif

		//--- new
		typedef CONT_T<
			tPlug<ELEM_T>,
			std::allocator< tPlug<ELEM_T> >
		> dContainer;	//!<

		typedef CONT_T<
			ELEM_T,
			std::allocator<ELEM_T>
		> dBaseContainer;

		typedef tCoolItr< dContainer > dItr;
		typedef tCoolItr_const< dContainer > dConstItr;

		tPlugLinearContainer();
		tPlugLinearContainer(const dBaseContainer &pCopyMe);
		virtual ~tPlugLinearContainer();

		dItr getItr();
		dConstItr getConstItr() const;

									tPlugLinearContainer<ELEM_T, CONT_T>& operator= (const tPlugLinearContainer<ELEM_T, CONT_T> &pCopyMe);
									tPlugLinearContainer<ELEM_T, CONT_T>& operator+= (const tPlugLinearContainer<ELEM_T, CONT_T> &pCopyMe);
									tPlugLinearContainer<ELEM_T, CONT_T>& operator= (const dBaseContainer &pCopyMe);
									tPlugLinearContainer<ELEM_T, CONT_T>& operator+= (const dBaseContainer &pCopyMe);
									tPlugLinearContainer<ELEM_T, CONT_T>& operator+= (const ELEM_T &pCopyMe);
		template<typename OTHER>	tPlugLinearContainer<ELEM_T, CONT_T>& operator= (const OTHER &pCopyMe);
		template<typename OTHER>	tPlugLinearContainer<ELEM_T, CONT_T>& operator+= (const OTHER &pCopyMe);

	protected:

#		ifdef GT_THREADS
			typedef boost::lock_guard<boost::recursive_mutex> dLock;

			static const size_t NO_CLEAR;

			boost::recursive_mutex mGuard;
			size_t mClearTo;	//!< We may have wanted to clear the container to fill it with new plugs. Instead we append, and on update we clear away the old nodes.

			virtual void shadowAppends(cBase_plug *pWriteTo, dConSig pSig);
			virtual void appendShadow(cBase_plug *pReadFrom, dConSig pSig);
			virtual void readShadow(cBase_plug *pWriteTo, dConSig pCon);
			virtual void writeShadow(const cBase_plug *pReadFrom, dConSig pCon);
#		endif

	private:
		dContainer mContainer;	//!<

	};


}



///////////////////////////////////////////////////////////////////////////////////
// Template definitions.
namespace gt{


	//-----------------------------------------------------------------------------------------------
#	ifdef GT_THREADS
		template< typename ELEM_T, template<typename, typename> class CONT_T >
		const size_t tPlugLinearContainer<ELEM_T, CONT_T>::NO_CLEAR = 0;
#	endif

	template< typename ELEM_T, template<typename, typename> class CONT_T >
	tPlugLinearContainer<ELEM_T, CONT_T>::tPlugLinearContainer(){
#		ifdef GT_THREADS
			mClearTo = NO_CLEAR;
#		endif
	}

	template< typename ELEM_T, template<typename, typename> class CONT_T >
	tPlugLinearContainer<ELEM_T, CONT_T>::tPlugLinearContainer(const dBaseContainer &pCopyMe){
		for(typename dBaseContainer::const_iterator itr = pCopyMe.begin(); itr != pCopyMe.end(); ++itr)
			mContainer.push_back( tPlug<ELEM_T>(*itr) );
	}

	template< typename ELEM_T, template<typename, typename> class CONT_T >
	tPlugLinearContainer<ELEM_T, CONT_T>::~tPlugLinearContainer(){
	}

	template< typename ELEM_T, template<typename, typename> class CONT_T >
	typename tPlugLinearContainer<ELEM_T, CONT_T>::dItr
	tPlugLinearContainer<ELEM_T, CONT_T>::getItr(){
		return dItr(&mContainer);
	}

	template< typename ELEM_T, template<typename, typename> class CONT_T >
	typename tPlugLinearContainer<ELEM_T, CONT_T>::dConstItr
	tPlugLinearContainer<ELEM_T, CONT_T>::getConstItr() const{
		return dConstItr(&mContainer);
	}

	template< typename ELEM_T, template<typename, typename> class CONT_T >
	tPlugLinearContainer<ELEM_T, CONT_T>&
	tPlugLinearContainer<ELEM_T, CONT_T>::operator= (const tPlugLinearContainer<ELEM_T, CONT_T> &pCopyMe){
		if(&pCopyMe != this)
			cAnyOp::assign(pCopyMe.mContainer, &mContainer, getType());
		return *this;
	}

	template< typename ELEM_T, template<typename, typename> class CONT_T >
	tPlugLinearContainer<ELEM_T, CONT_T>&
	tPlugLinearContainer<ELEM_T, CONT_T>::operator+= (
		const tPlugLinearContainer<ELEM_T, CONT_T> &pCopyMe
	){
		if(&pCopyMe != this)
			cAnyOp::append(pCopyMe.mContainer, &mContainer, getType());
		return *this;
	}

	template< typename ELEM_T, template<typename, typename> class CONT_T >
	tPlugLinearContainer<ELEM_T, CONT_T>&
	tPlugLinearContainer<ELEM_T, CONT_T>::operator= (
		const tPlugLinearContainer<ELEM_T, CONT_T>::dBaseContainer &pCopyMe
	){
		mContainer.clear();
		return operator+=(pCopyMe);
	}

	template< typename ELEM_T, template<typename, typename> class CONT_T >
	tPlugLinearContainer<ELEM_T, CONT_T>&
	tPlugLinearContainer<ELEM_T, CONT_T>::operator+= (
		const tPlugLinearContainer<ELEM_T, CONT_T>::dBaseContainer &pCopyMe
	){
		for(typename dBaseContainer::const_iterator itr = pCopyMe.begin(); itr != pCopyMe.end(); ++itr)
			mContainer.push_back( tPlug<ELEM_T>(*itr) );

		return *this;
	}

	template< typename ELEM_T, template<typename, typename> class CONT_T >
	tPlugLinearContainer<ELEM_T, CONT_T>&
	tPlugLinearContainer<ELEM_T, CONT_T>::operator+= (
		const ELEM_T &pCopyMe
	){
		mContainer.push_back( tPlug<ELEM_T>(pCopyMe) );
		return *this;
	}

	template< typename ELEM_T, template<typename, typename> class CONT_T >
	template<typename OTHER>
	tPlugLinearContainer<ELEM_T, CONT_T>&
	tPlugLinearContainer<ELEM_T, CONT_T>::operator= (const OTHER &pCopyMe){
		cAnyOp::assign(pCopyMe, &mContainer, getType());
		return *this;
	}

	template< typename ELEM_T, template<typename, typename> class CONT_T >
	template<typename OTHER>
	tPlugLinearContainer<ELEM_T, CONT_T>&
	tPlugLinearContainer<ELEM_T, CONT_T>::operator+= (const OTHER &pCopyMe){
		cAnyOp::append(pCopyMe, &mContainer, getType());
		return *this;
	}

	template< typename ELEM_T, template<typename, typename> class CONT_T >
	void
	tPlugLinearContainer<ELEM_T, CONT_T>::assignTo(void *pTo, dPlugType pType) const{
		cAnyOp::assign(mContainer, pTo, pType);
	}

	template< typename ELEM_T, template<typename, typename> class CONT_T >
	void
	tPlugLinearContainer<ELEM_T, CONT_T>::appendTo(void *pTo, dPlugType pType) const{
		cAnyOp::append(mContainer, pTo, pType);
	}

	template< typename ELEM_T, template<typename, typename> class CONT_T >
	void
	tPlugLinearContainer<ELEM_T, CONT_T>::save(cByteBuffer* pSaveHere){
		PROFILE;
#		ifdef GT_THREADS
			dLock lock(mGuard);
#		endif

		size_t s = mContainer.size();
		pSaveHere->add( &s );
		for(typename dContainer::iterator itr = mContainer.begin(); itr != mContainer.end(); ++itr)
			itr->save(pSaveHere);
	}

	template< typename ELEM_T, template<typename, typename> class CONT_T >
	void
	tPlugLinearContainer<ELEM_T, CONT_T>::loadEat(cByteBuffer* pChewToy, dReloadMap *aReloads){
		PROFILE;
#		ifdef GT_THREADS
			dLock lock(mGuard);
#		endif

		typedef typename dContainer::value_type element;

		clear();

		size_t s=0, i=0;
		pChewToy->trimHead( pChewToy->fill(&s) );
		while(i < s){
			mContainer.push_back(element());
			mContainer.back().loadEat(pChewToy, aReloads);
			++i;
		}
	}

	template< typename ELEM_T, template<typename, typename> class CONT_T >
	bool
	tPlugLinearContainer<ELEM_T, CONT_T>::operator== (const cBase_plug &pCompare) const{
		return (getType() == pCompare.getType());
	}

	template< typename ELEM_T, template<typename, typename> class CONT_T >
	cBase_plug&
	tPlugLinearContainer<ELEM_T, CONT_T>::operator= (const cBase_plug &pCopyMe){
		if(&pCopyMe != this)
			pCopyMe.assignTo(&mContainer, getType());
		return *this;
	}

	template< typename ELEM_T, template<typename, typename> class CONT_T >
	cBase_plug&
	tPlugLinearContainer<ELEM_T, CONT_T>::operator+= (const cBase_plug &pCopyMe){
		if(&pCopyMe != this)
			pCopyMe.appendTo(&mContainer, getType());
		return *this;
	}

	template< typename ELEM_T, template<typename, typename> class CONT_T >
	dPlugType
	tPlugLinearContainer<ELEM_T, CONT_T>::getType() const{
		return genPlugType< dContainer >();
	}

	template< typename ELEM_T, template<typename, typename> class CONT_T >
	size_t
	tPlugLinearContainer<ELEM_T, CONT_T>::getCount() const{
#		ifdef GT_THREADS
			if(mClearTo != NO_CLEAR)
				return mContainer.size() - mClearTo;
#		endif

		return mContainer.size();
	}

	template< typename ELEM_T, template<typename, typename> class CONT_T >
	cBase_plug*
	tPlugLinearContainer<ELEM_T, CONT_T>::getPlug(size_t idx){
#		ifdef GT_THREADS
			if(mClearTo != NO_CLEAR)
				idx += mClearTo;
#		endif
		return tGetterPlug< tPlug<ELEM_T>, CONT_T>::get(mContainer, idx);
	}

	template< typename ELEM_T, template<typename, typename> class CONT_T >
	const cBase_plug*
	tPlugLinearContainer<ELEM_T, CONT_T>::getPlugConst(size_t idx) const{
#		ifdef GT_THREADS
			if(mClearTo != NO_CLEAR)
				idx += mClearTo;
#		endif
		return tGetterPlug< tPlug<ELEM_T>, CONT_T>::getConst(mContainer, idx);
	}

	template< typename ELEM_T, template<typename, typename> class CONT_T >
	void
	tPlugLinearContainer<ELEM_T, CONT_T>::add(const cBase_plug &addMe){
		mContainer.push_back(tPlug<ELEM_T>(addMe));
	}

	template< typename ELEM_T, template<typename, typename> class CONT_T >
	void
	tPlugLinearContainer<ELEM_T, CONT_T>::clear(){
#		ifdef GT_THREADS
			mClearTo = mContainer.size();
#		else
			mContainer.clear();
#		endif
	}


#ifdef GT_THREADS

	template< typename ELEM_T, template<typename, typename> class CONT_T >
	void
	tPlugLinearContainer<ELEM_T, CONT_T>::updateStart(){
		PROFILE;
		dLock lock(mGuard);
		if(mClearTo != NO_CLEAR){
			if(mClearTo == 1){
				mContainer.erase(mContainer.begin());
			}else{
				typename dContainer::iterator end;
				std::advance(end, mClearTo-1);
				mContainer.erase(mContainer.begin(), end);
			}
			mClearTo = NO_CLEAR;
		}

		for(typename dContainer::iterator itr=mContainer.begin(); itr != mContainer.end(); ++itr)
			itr->updateStart();
	}

	template< typename ELEM_T, template<typename, typename> class CONT_T >
	void
	tPlugLinearContainer<ELEM_T, CONT_T>::updateFinish(){
		PROFILE;
		dLock lock(mGuard);
		for(typename dContainer::iterator itr=mContainer.begin(); itr != mContainer.end(); ++itr)
			itr->updateFinish();
	}

	template< typename ELEM_T, template<typename, typename> class CONT_T >
	void
	tPlugLinearContainer<ELEM_T, CONT_T>::readShadow(cBase_plug *pWriteTo, dConSig aCon){
		internalAssignTo(pWriteTo, pWriteTo->getType(), aCon);
	}

	template< typename ELEM_T, template<typename, typename> class CONT_T >
	void
	tPlugLinearContainer<ELEM_T, CONT_T>::writeShadow(const cBase_plug *pReadFrom, dConSig aCon){
		internalAssign(*pReadFrom, aCon);
	}


	template< typename ELEM_T, template<typename, typename> class CONT_T >
	void
	tPlugLinearContainer<ELEM_T, CONT_T>::shadowAppends(cBase_plug *pWriteTo, dConSig pSig){
		internalAssignTo(pWriteTo, pWriteTo->getType(), pSig);
	}

	template< typename ELEM_T, template<typename, typename> class CONT_T >
	void
	tPlugLinearContainer<ELEM_T, CONT_T>::appendShadow(cBase_plug *pReadFrom, dConSig pSig){
		internalAssign(*pReadFrom, pSig);
	}

#endif

}



#endif
