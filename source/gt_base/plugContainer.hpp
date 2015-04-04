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

		typedef CONT_T<
			tPlug<ELEM_T>,
			std::allocator< tPlug<ELEM_T> >
		> dContainer;

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

	private:
		dContainer mContainer;	//!<

	};
}



///////////////////////////////////////////////////////////////////////////////////
// Template definitions.
namespace gt{


	template< typename ELEM_T, template<typename, typename> class CONT_T >
	tPlugLinearContainer<ELEM_T, CONT_T>::tPlugLinearContainer(){
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
	tPlugLinearContainer<ELEM_T, CONT_T>::operator= (
		const tPlugLinearContainer<ELEM_T, CONT_T> &pCopyMe
	){
		PLUG_DATALOCK;
		if(&pCopyMe != this)
			cAnyOp::assign(pCopyMe.mContainer, &mContainer, getType());
		return *this;
	}

	template< typename ELEM_T, template<typename, typename> class CONT_T >
	tPlugLinearContainer<ELEM_T, CONT_T>&
	tPlugLinearContainer<ELEM_T, CONT_T>::operator+= (
		const tPlugLinearContainer<ELEM_T, CONT_T> &pCopyMe
	){
		PLUG_DATALOCK;
		if(&pCopyMe != this)
			cAnyOp::append(pCopyMe.mContainer, &mContainer, getType());
		return *this;
	}

	template< typename ELEM_T, template<typename, typename> class CONT_T >
	tPlugLinearContainer<ELEM_T, CONT_T>&
	tPlugLinearContainer<ELEM_T, CONT_T>::operator= (
		const tPlugLinearContainer<ELEM_T, CONT_T>::dBaseContainer &pCopyMe
	){
		PLUG_DATALOCK;
		mContainer.clear();
		return operator+=(pCopyMe);
	}

	template< typename ELEM_T, template<typename, typename> class CONT_T >
	tPlugLinearContainer<ELEM_T, CONT_T>&
	tPlugLinearContainer<ELEM_T, CONT_T>::operator+= (
		const tPlugLinearContainer<ELEM_T, CONT_T>::dBaseContainer &pCopyMe
	){
		PLUG_DATALOCK;
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
		PLUG_DATALOCK;
		cAnyOp::assign(pCopyMe, &mContainer, getType());
		return *this;
	}

	template< typename ELEM_T, template<typename, typename> class CONT_T >
	template<typename OTHER>
	tPlugLinearContainer<ELEM_T, CONT_T>&
	tPlugLinearContainer<ELEM_T, CONT_T>::operator+= (const OTHER &pCopyMe){
		PLUG_DATALOCK;
		cAnyOp::append(pCopyMe, &mContainer, getType());
		return *this;
	}

	template< typename ELEM_T, template<typename, typename> class CONT_T >
	void
	tPlugLinearContainer<ELEM_T, CONT_T>::assignTo(void *pTo, dPlugType pType) const{
		PLUG_DATALOCK;
		cAnyOp::assign(mContainer, pTo, pType);
	}

	template< typename ELEM_T, template<typename, typename> class CONT_T >
	void
	tPlugLinearContainer<ELEM_T, CONT_T>::appendTo(void *pTo, dPlugType pType) const{
		PLUG_DATALOCK;
		cAnyOp::append(mContainer, pTo, pType);
	}

	template< typename ELEM_T, template<typename, typename> class CONT_T >
	void
	tPlugLinearContainer<ELEM_T, CONT_T>::save(cByteBuffer* pSaveHere){
		PLUG_DATALOCK;
		size_t s = mContainer.size();
		pSaveHere->add( &s );
		for(typename dContainer::iterator itr = mContainer.begin(); itr != mContainer.end(); ++itr)
			itr->save(pSaveHere);
	}

	template< typename ELEM_T, template<typename, typename> class CONT_T >
	void
	tPlugLinearContainer<ELEM_T, CONT_T>::loadEat(cByteBuffer* pChewToy, dReloadMap *aReloads){
		PLUG_DATALOCK;
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
		return mContainer.size();
	}

	template< typename ELEM_T, template<typename, typename> class CONT_T >
	void
	tPlugLinearContainer<ELEM_T, CONT_T>::add(const cBase_plug &addMe){
		mContainer.push_back(tPlug<ELEM_T>(addMe));
	}

	template< typename ELEM_T, template<typename, typename> class CONT_T >
	void
	tPlugLinearContainer<ELEM_T, CONT_T>::clear(){
		mContainer.clear();
	}



}



#endif
