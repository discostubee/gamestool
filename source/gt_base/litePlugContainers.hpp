/*
 * !\file	lead.hpp
 * !\brief
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

#ifndef	LITEPLUGCONTAINERS_HPP
#define LITEPLUGCONTAINERS_HPP

#include "basePlug.hpp"
#include "plugContainerGetter.hpp"

///////////////////////////////////////////////////////////////////////////////////
// Classes
namespace gt{

	//!\brief	Be careful with these.
	template<
		typename ELEM_T,
		template<typename, typename> class CONT_T
	>
	class tLitePlugLinerContainer : public cBase_plugContainer{
	public:
		typedef CONT_T<
			tLitePlug<ELEM_T>,
			std::allocator< tLitePlug<ELEM_T> >
		> dContainer;

		typedef CONT_T<
			ELEM_T,
			std::allocator<ELEM_T>
		> dBaseContainer;

		tLitePlugLinerContainer(dBaseContainer * pRef);
		~tLitePlugLinerContainer();

		dPlugType getType() const;
		void assignTo(void *pTo, dPlugType pType) const;
		void appendTo(void *pTo, dPlugType pType) const;
		void save(cByteBuffer* pSaveHere) { DONT_USE_THIS; }
		void loadEat(cByteBuffer* pChewToy, dReloadMap *aReloads = NULL) { DONT_USE_THIS; }
		bool operator== (const cBase_plug &pD) const;
		cBase_plug& operator= (const cBase_plug &pD);	//!< Assigns only the content, should not copy any linked lead info.
		cBase_plug& operator+= (const cBase_plug &pD);

		//- Would like to protect these, but can't.
		size_t getCount() const;
		cBase_plug* getPlug(size_t idx);
		const cBase_plug* getPlugConst(size_t idx) const;
		void add(const cBase_plug &addMe);
		void clear();

#	ifdef GT_THREADS
		void updateStart() { DONT_USE_THIS; }
		void updateFinish() { DONT_USE_THIS; }
#	endif

	private:
		dContainer mContainer;

#	ifdef GT_THREADS
		void readShadow(cBase_plug *pWriteTo, dConSig aCon) { DONT_USE_THIS; }
		void writeShadow(const cBase_plug *pReadFrom, dConSig aCon) { DONT_USE_THIS; }
		virtual void shadowAppends(cBase_plug *pWriteTo, dConSig pSig) { DONT_USE_THIS; }
		virtual void appendShadow(cBase_plug *pReadFrom, dConSig pSig) { DONT_USE_THIS; }
#	endif

	};

}

///////////////////////////////////////////////////////////////////////////////////
// Classes
namespace gt{

template< typename ELEM_T, template<typename, typename> class CONT_T >
tLitePlugLinerContainer<ELEM_T, CONT_T>::tLitePlugLinerContainer(dBaseContainer * pRef)
{
	for(typename dBaseContainer::iterator itr = pRef->begin(); itr != pRef->end(); ++itr)
		mContainer.push_back( tLitePlug<ELEM_T>(*itr) );
}

template< typename ELEM_T, template<typename, typename> class CONT_T >
tLitePlugLinerContainer<ELEM_T, CONT_T>::~tLitePlugLinerContainer(){
}

template< typename ELEM_T, template<typename, typename> class CONT_T >
dPlugType
tLitePlugLinerContainer<ELEM_T, CONT_T>::getType() const{
	return genPlugType< dBaseContainer >();
}

template< typename ELEM_T, template<typename, typename> class CONT_T >
void
tLitePlugLinerContainer<ELEM_T, CONT_T>::assignTo(void *pTo, dPlugType pType) const{
	cAnyOp::assign(mContainer, pTo, pType);
}

template< typename ELEM_T, template<typename, typename> class CONT_T >
void
tLitePlugLinerContainer<ELEM_T, CONT_T>::appendTo(void *pTo, dPlugType pType) const{
	cAnyOp::append(mContainer, pTo, pType);
}

template< typename ELEM_T, template<typename, typename> class CONT_T >
bool
tLitePlugLinerContainer<ELEM_T, CONT_T>::operator== (const cBase_plug &pD) const{
	return getType() == pD.getType();
}

template< typename ELEM_T, template<typename, typename> class CONT_T >
cBase_plug&
tLitePlugLinerContainer<ELEM_T, CONT_T>::operator= (const cBase_plug &pD){
	if(&pD != this)
		pD.assignTo(mContainer, getType());

	return *this;
}

template< typename ELEM_T, template<typename, typename> class CONT_T >
cBase_plug&
tLitePlugLinerContainer<ELEM_T, CONT_T>::operator+= (const cBase_plug &pD){
	if(&pD != this)
		pD.appendTo(mContainer, getType());

	return *this;
}

template< typename ELEM_T, template<typename, typename> class CONT_T >
size_t
tLitePlugLinerContainer<ELEM_T, CONT_T>::getCount() const{
	return mContainer.size();
}

template< typename ELEM_T, template<typename, typename> class CONT_T >
cBase_plug*
tLitePlugLinerContainer<ELEM_T, CONT_T>::getPlug(size_t idx){
	return tGetterPlug<ELEM_T, CONT_T>::get(mContainer, idx);
}

template< typename ELEM_T, template<typename, typename> class CONT_T >
const cBase_plug*
tLitePlugLinerContainer<ELEM_T, CONT_T>::getPlugConst(size_t idx) const{
	return tGetterPlug<ELEM_T, CONT_T>::getConst(mContainer, idx);
}

template< typename ELEM_T, template<typename, typename> class CONT_T >
void
tLitePlugLinerContainer<ELEM_T, CONT_T>::add(const cBase_plug &addMe){
	addMe.appendTo(mContainer, getType());
}

template< typename ELEM_T, template<typename, typename> class CONT_T >
void
tLitePlugLinerContainer<ELEM_T, CONT_T>::clear(){
	mContainer.clear();
}



}


#endif
