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
// class
namespace gt{

	//----------------------------------------------------------------------------------------------------------------
	//!\brief	Allows for easy specialisation.
	template<
		typename PLUG_T,
		template<typename, typename> class CONT_T
	>
	class tGetterPlug{
	public:
		typedef CONT_T<
			tPlug<PLUG_T>,
			std::allocator<PLUG_T>
		> dContainer;

		static
		cBase_plug*
		get(dContainer &from, size_t idx) {DONT_USE_THIS; return NULL;}

		static
		const cBase_plug*
		getConst(const dContainer &from, size_t idx) {DONT_USE_THIS; return NULL;}

		static
		tPlug<PLUG_T>&
		getActual(dContainer &from, size_t idx) {DONT_USE_THIS; return tPlug<PLUG_T>(); }
	};

	//----------------------------------------------------------------------------------------------------------------
	//!\brief	Works with things like vectors and linked lists which don't have any special uses like a map does.
	template<
		typename PLUG_T,
		template<typename, typename> class CONT_T
	>
	class tPlugLinearContainer : public cBasePlugContainer{
	public:

		//--- override container
		size_t getCount() const;
		cBase_plug* getPlug(size_t idx);
		const cBase_plug* getPlugConst(size_t idx) const;
		void add(const cBase_plug &addMe);
		void clear();

		cBasePlugContainer& operator= (const cBasePlugContainer &pCopyMe);
		cBasePlugContainer& operator+= (const cBasePlugContainer &pCopyMe);

		//--- override base plug
		void assignTo(void *pTo, dPlugType pType) const;
		void appendTo(void *pTo, dPlugType pType) const;
		void save(cByteBuffer* pSaveHere);
		void loadEat(cByteBuffer* pChewToy, dReloadMap *aReloads = NULL);

		bool operator== (const cBase_plug &pD) const;
		cBase_plug& operator= (const cBase_plug &pD);
		cBase_plug& operator+= (const cBase_plug &pD);

#		ifdef GT_THREADS
			virtual void updateStart();
			virtual void updateFinish();
#		endif

		//--- new
		typedef CONT_T<
			tPlug<PLUG_T>,
			std::allocator<PLUG_T>
		> dContainer;	//!< The default template parameter doesn't appear to work when using it for template-template parameters.

		typedef CONT_T<
			PLUG_T,
			std::allocator<PLUG_T>
		> dBaseContainer;

		typedef tCoolItr< dContainer > dItr;

		tPlugLinearContainer();
		virtual ~tPlugLinearContainer();

		tPlug<PLUG_T>& getActual(size_t idx);
		dItr getItr();

									tPlugLinearContainer<PLUG_T, CONT_T>& operator= (const tPlugLinearContainer<PLUG_T, CONT_T> &pCopyMe);
									tPlugLinearContainer<PLUG_T, CONT_T>& operator+= (const tPlugLinearContainer<PLUG_T, CONT_T> &pCopyMe);
									tPlugLinearContainer<PLUG_T, CONT_T>& operator= (const dBaseContainer &pCopyMe);
									tPlugLinearContainer<PLUG_T, CONT_T>& operator+= (const dBaseContainer &pCopyMe);
		template<typename OTHER>	tPlugLinearContainer<PLUG_T, CONT_T>& operator= (const OTHER &pCopyMe);
		template<typename OTHER>	tPlugLinearContainer<PLUG_T, CONT_T>& operator+= (const OTHER &pCopyMe);

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

		void internalAssign(const cBase_plug &pD, bool pClear, dConSig aCon = SL_NO_ENTRY);
		void internalAssignTo(void *pTo, dPlugType pType, bool pClear, dConSig aCon = SL_NO_ENTRY) const;
	};

}


///////////////////////////////////////////////////////////////////////////////////
// Templates
namespace gt{


	//-----------------------------------------------------------------------------------------------
#	ifdef GT_THREADS
		template< typename PLUG_T, template<typename, typename> class CONT_T >
		const size_t tPlugLinearContainer<PLUG_T, CONT_T>::NO_CLEAR = static_cast<size_t>(-1);
#	endif

	template< typename PLUG_T, template<typename, typename> class CONT_T >
	tPlugLinearContainer<PLUG_T, CONT_T>::tPlugLinearContainer(){
#		ifdef GT_THREADS
			mClearTo = NO_CLEAR;
#		endif
	}

	template< typename PLUG_T, template<typename, typename> class CONT_T >
	tPlugLinearContainer<PLUG_T, CONT_T>::~tPlugLinearContainer(){
	}

	template< typename PLUG_T, template<typename, typename> class CONT_T >
	tPlug<PLUG_T>&
	tPlugLinearContainer<PLUG_T, CONT_T>::getActual(size_t idx){
		return tGetterPlug<PLUG_T, CONT_T>::getActual(mContainer, idx);
	}

	template< typename PLUG_T, template<typename, typename> class CONT_T >
	typename tPlugLinearContainer<PLUG_T, CONT_T>::dItr
	tPlugLinearContainer<PLUG_T, CONT_T>::getItr(){
		return dItr(&mContainer);
	}

	template< typename PLUG_T, template<typename, typename> class CONT_T >
	tPlugLinearContainer<PLUG_T, CONT_T>&
	tPlugLinearContainer<PLUG_T, CONT_T>::operator= (const tPlugLinearContainer<PLUG_T, CONT_T> &pCopyMe){
		ASRT_NOTSELF(&pCopyMe);

		mContainer.clear();
		return operator+=(pCopyMe);
	}

	template< typename PLUG_T, template<typename, typename> class CONT_T >
	tPlugLinearContainer<PLUG_T, CONT_T>&
	tPlugLinearContainer<PLUG_T, CONT_T>::operator+= (const tPlugLinearContainer<PLUG_T, CONT_T> &pCopyMe){
		ASRT_NOTSELF(&pCopyMe);

		for(typename dContainer::const_iterator itr = pCopyMe.mContainer.begin(); itr != pCopyMe.mContainer.end();  ++itr)
			mContainer.push_back(*itr);

		return *this;
	}

	template< typename PLUG_T, template<typename, typename> class CONT_T >
	tPlugLinearContainer<PLUG_T, CONT_T>&
	tPlugLinearContainer<PLUG_T, CONT_T>::operator= (
		const tPlugLinearContainer<PLUG_T, CONT_T>::dBaseContainer &pCopyMe
	){
		PROFILE;
#		ifdef GT_THREADS
			dLock lock(mGuard);
#		endif

		mContainer.clear();
		return tPlugLinearContainer<PLUG_T, CONT_T>::operator+=(pCopyMe);
	}

	template< typename PLUG_T, template<typename, typename> class CONT_T >
	tPlugLinearContainer<PLUG_T, CONT_T>&
	tPlugLinearContainer<PLUG_T, CONT_T>::operator+= (
		const tPlugLinearContainer<PLUG_T, CONT_T>::dBaseContainer &pCopyMe
	){
		PROFILE;
#		ifdef GT_THREADS
			dLock lock(mGuard);
#		endif

		for(typename dBaseContainer::const_iterator itr = pCopyMe.begin(); itr != pCopyMe.end();  ++itr)
			mContainer.push_back(tPlug<PLUG_T>(*itr));

		return *this;
	}

	template< typename PLUG_T, template<typename, typename> class CONT_T >
	template<typename OTHER>
	tPlugLinearContainer<PLUG_T, CONT_T>&
	tPlugLinearContainer<PLUG_T, CONT_T>::operator= (const OTHER &pCopyMe){
		PROFILE;
#		ifdef GT_THREADS
			dLock lock(mGuard);
			mClearTo = mContainer.size();
#		endif

		mContainer.clear();
		mContainer.push_back(tPlug<PLUG_T>(pCopyMe));
		return *this;
	}

	template< typename PLUG_T, template<typename, typename> class CONT_T >
	template<typename OTHER>
	tPlugLinearContainer<PLUG_T, CONT_T>&
	tPlugLinearContainer<PLUG_T, CONT_T>::operator+= (const OTHER &pCopyMe){
		PROFILE;
#		ifdef GT_THREADS
			dLock lock(mGuard);
#		endif

		mContainer.push_back(tPlug<PLUG_T>(pCopyMe));
		return *this;
	}

	template< typename PLUG_T, template<typename, typename> class CONT_T >
	void
	tPlugLinearContainer<PLUG_T, CONT_T>::assignTo(void *aTo, dPlugType aType) const{
		internalAssignTo(aTo, aType, true);
	}

	template< typename PLUG_T, template<typename, typename> class CONT_T >
	void
	tPlugLinearContainer<PLUG_T, CONT_T>::appendTo(void *aTo, dPlugType aType) const{
		internalAssignTo(aTo, aType, false);
	}

	template< typename PLUG_T, template<typename, typename> class CONT_T >
	void
	tPlugLinearContainer<PLUG_T, CONT_T>::save(cByteBuffer* pSaveHere){
		PROFILE;
#		ifdef GT_THREADS
			dLock lock(mGuard);
#		endif

		size_t s = mContainer.size();
		pSaveHere->add( &s );
		for(typename dContainer::iterator itr = mContainer.begin(); itr != mContainer.end(); ++itr)
			itr->save(pSaveHere);
	}

	template< typename PLUG_T, template<typename, typename> class CONT_T >
	void
	tPlugLinearContainer<PLUG_T, CONT_T>::loadEat(cByteBuffer* pChewToy, dReloadMap *aReloads){
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

	template< typename PLUG_T, template<typename, typename> class CONT_T >
	bool
	tPlugLinearContainer<PLUG_T, CONT_T>::operator== (const cBase_plug &pD) const{
		return (getType() == pD.getType());
	}

	template< typename PLUG_T, template<typename, typename> class CONT_T >
	cBase_plug&
	tPlugLinearContainer<PLUG_T, CONT_T>::operator= (const cBase_plug &pD){
		NOTSELF(&pD);
		internalAssign(pD, true);
		return *this;
	}

	template< typename PLUG_T, template<typename, typename> class CONT_T >
	cBase_plug&
	tPlugLinearContainer<PLUG_T, CONT_T>::operator+= (const cBase_plug &pD){
		NOTSELF(&pD);
		internalAssign(pD, false);
		return *this;
	}

	template< typename PLUG_T, template<typename, typename> class CONT_T >
	size_t
	tPlugLinearContainer<PLUG_T, CONT_T>::getCount() const{
#		ifdef GT_THREADS
			if(mClearTo != NO_CLEAR)
				return mContainer.size() - mClearTo;
#		endif

		return mContainer.size();
	}

	template< typename PLUG_T, template<typename, typename> class CONT_T >
	cBase_plug*
	tPlugLinearContainer<PLUG_T, CONT_T>::getPlug(size_t idx){
#		ifdef GT_THREADS
			if(mClearTo != NO_CLEAR)
				idx += mClearTo;
#		endif
		return tGetterPlug<PLUG_T, CONT_T>::get(mContainer, idx);
	}

	template< typename PLUG_T, template<typename, typename> class CONT_T >
	const cBase_plug*
	tPlugLinearContainer<PLUG_T, CONT_T>::getPlugConst(size_t idx) const{
#		ifdef GT_THREADS
			if(mClearTo != NO_CLEAR)
				idx += mClearTo;
#		endif
		return tGetterPlug<PLUG_T, CONT_T>::getConst(mContainer, idx);
	}

	template< typename PLUG_T, template<typename, typename> class CONT_T >
	void
	tPlugLinearContainer<PLUG_T, CONT_T>::add(const cBase_plug &addMe){
		mContainer.push_back(tPlug<PLUG_T>(addMe));
	}

	template< typename PLUG_T, template<typename, typename> class CONT_T >
	void
	tPlugLinearContainer<PLUG_T, CONT_T>::clear(){
#		ifdef GT_THREADS
			mClearTo = mContainer.size();
#		else
			mContainer.clear();
#		endif
	}

	template< typename PLUG_T, template<typename, typename> class CONT_T >
	cBasePlugContainer&
	tPlugLinearContainer<PLUG_T, CONT_T>::operator= (const cBasePlugContainer &pCopyMe){
		NOTSELF(&pCopyMe);
		internalAssign(pCopyMe, true);
		return *this;
	}

	template< typename PLUG_T, template<typename, typename> class CONT_T >
	cBasePlugContainer&
	tPlugLinearContainer<PLUG_T, CONT_T>::operator+= (const cBasePlugContainer &pCopyMe){
		NOTSELF(&pCopyMe);
		internalAssign(pCopyMe, false);
		return *this;
	}

	template< typename PLUG_T, template<typename, typename> class CONT_T >
	void
	tPlugLinearContainer<PLUG_T, CONT_T>::internalAssign(const cBase_plug &pD, bool pClear, dConSig aCon){
		PROFILE;
#		ifdef GT_THREADS
			dLock lock(mGuard);
#		endif

		if(pClear)
			clear();

		if(pD.getType() == getType()){
			const cBasePlugContainer *tmp = dynamic_cast<const cBasePlugContainer*>(&pD);
			for(size_t i=0; i < tmp->getCount(); ++i)
				mContainer.push_back(*tmp->getPlugConst(i));

		}else{
			mContainer.push_back(pD);
		}
	}

	template< typename PLUG_T, template<typename, typename> class CONT_T >
	void
	tPlugLinearContainer<PLUG_T, CONT_T>::internalAssignTo(void *pTo, dPlugType pType, bool pClear, dConSig aCon) const{
		PROFILE;

		if(mContainer.empty())
			return;

		if(pType == getType()){
			cBasePlugContainer *tmp = static_cast<cBasePlugContainer*>(pTo);

			if(pClear)
				tmp->clear();

#			ifdef GT_THREADS
				size_t stop =0;
#			endif

			for(typename dContainer::const_iterator itr = mContainer.begin(); itr != mContainer.end(); ++itr){
				tmp->add(*itr);
#				ifdef GT_THREADS
					if(mClearTo != NO_CLEAR && stop == mClearTo +1)
						break;
#				endif
			}

		}else{
			mContainer.front().assignTo(pTo, pType);
		}
	}

#ifdef GT_THREADS

	template< typename PLUG_T, template<typename, typename> class CONT_T >
	void
	tPlugLinearContainer<PLUG_T, CONT_T>::updateStart(){
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

	template< typename PLUG_T, template<typename, typename> class CONT_T >
	void
	tPlugLinearContainer<PLUG_T, CONT_T>::updateFinish(){
		PROFILE;
		dLock lock(mGuard);
		for(typename dContainer::iterator itr=mContainer.begin(); itr != mContainer.end(); ++itr)
			itr->updateFinish();
	}

	template< typename PLUG_T, template<typename, typename> class CONT_T >
	void
	tPlugLinearContainer<PLUG_T, CONT_T>::readShadow(cBase_plug *pWriteTo, dConSig aCon){
		internalAssignTo(pWriteTo, pWriteTo->getType(), true, aCon);
	}

	template< typename PLUG_T, template<typename, typename> class CONT_T >
	void
	tPlugLinearContainer<PLUG_T, CONT_T>::writeShadow(const cBase_plug *pReadFrom, dConSig aCon){
		internalAssign(*pReadFrom, true, aCon);
	}


	template< typename PLUG_T, template<typename, typename> class CONT_T >
	void
	tPlugLinearContainer<PLUG_T, CONT_T>::shadowAppends(cBase_plug *pWriteTo, dConSig pSig){
		internalAssignTo(pWriteTo, pWriteTo->getType(), false, pSig);
	}

	template< typename PLUG_T, template<typename, typename> class CONT_T >
	void
	tPlugLinearContainer<PLUG_T, CONT_T>::appendShadow(cBase_plug *pReadFrom, dConSig pSig){
		internalAssign(*pReadFrom, false, pSig);
	}

#endif

}

///////////////////////////////////////////////////////////////////////////////////
// specialisations.
namespace gt{

	template<typename PLUG_T>
	class tGetterPlug<PLUG_T, std::vector>{
	public:
		typedef std::vector<
			tPlug<PLUG_T>,
			std::allocator<PLUG_T>
		> dContainer;

		static
		cBase_plug*
		get(dContainer &from, size_t idx){
			ASRT_INRANGE(from, idx);
			return &from[idx];
		}

		static
		const cBase_plug*
		getConst(const dContainer &from, size_t idx){
			ASRT_INRANGE(from, idx);
			return &from[idx];
		}

		static
		tPlug<PLUG_T>&
		getActual(dContainer &from, size_t idx){
			ASRT_INRANGE(from, idx);
			return from[idx];
		}
	};

	template<typename PLUG_T>
	class tGetterPlug<PLUG_T, std::list>{
	public:
		typedef std::list<
			tPlug<PLUG_T>,
			std::allocator<PLUG_T>
		> dContainer;

		static
		cBase_plug*
		get(dContainer &from, size_t idx){
			ASRT_INRANGE(from, idx);
			typename dContainer::iterator itr( from.begin() );
			for(size_t i=0; i < idx; ++i)
				++itr;
			return &(*itr);
		}

		static
		const cBase_plug*
		getConst(const dContainer &from, size_t idx){
			ASRT_INRANGE(from, idx);
			typename dContainer::const_iterator itr( from.begin() );
			for(size_t i=0; i < idx; ++i)
				++itr;
			return &(*itr);
		}

		static
		tPlug<PLUG_T>&
		getActual(dContainer &from, size_t idx){
			ASRT_INRANGE(from, idx);
			typename dContainer::iterator itr( from.begin() );
			for(size_t i=0; i < idx; ++i)
				++itr;
			return (*itr);
		}
	};

}


#endif
