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

#ifndef	LITEPLUG_HPP
#define LITEPLUG_HPP

#include "basePlug.hpp"

///////////////////////////////////////////////////////////////////////////////////
// Classes
namespace gt{

	//----------------------------------------------------------------------------------------------------------------
	//!\brief	Used just for copying and appending. References data rather than copies.
	template<typename T>
	class tLitePlug: public tDataPlug<T>{
	public:
		tLitePlug(const tLitePlug &copyMe) : mRef( const_cast<T*>(copyMe.mRef) ) {}
		tLitePlug(T *aRef) : mRef(aRef) {}
		~tLitePlug() {}

		tLitePlug<T>& operator= (const tLitePlug<T> &aCopyMe) {
			ASRT_NOTSELF(&aCopyMe);
			mRef = aCopyMe.mRef;
			return *this;
		}

		T& get(){ return *mRef; }
		const T& getConst() const{ return *mRef; }

		//---
		void save(cByteBuffer* pSaveHere) { DONT_USE_THIS; }
		void loadEat(cByteBuffer* pChewToy, dReloadMap *aReloads = NULL) { DONT_USE_THIS; }

		#ifdef GT_THREADS
			void updateStart() { DONT_USE_THIS; }
			void updateFinish() { DONT_USE_THIS; }
		#endif

	protected:
	#		ifdef GT_THREADS
			void readShadow(cBase_plug *pWriteTo, dConSig aCon) { DONT_USE_THIS; }
			void writeShadow(const cBase_plug *pReadFrom, dConSig aCon) { DONT_USE_THIS; }
			virtual void shadowAppends(cBase_plug *pWriteTo, dConSig pSig) { DONT_USE_THIS; }
			virtual void appendShadow(cBase_plug *pReadFrom, dConSig pSig) { DONT_USE_THIS; }
	#		endif

	private:
		T *mRef;
	};

	//----------------------------------------------------------------------------------------------------------------
	//!\brief	Used just for copying and appending with constant.
	template<typename T>
	class tLitePlugConst: public tDataPlug<T>{
	public:
		tLitePlugConst(const T *aRef) : mRef(aRef) {}
		~tLitePlugConst(){}

		tLitePlug<T>& operator= (const tLitePlugConst<T> &aCopyMe) {
			ASRT_NOTSELF(&aCopyMe);
			mRef = aCopyMe.mRef;
			return *this;
		}

		T& get(){ DONT_USE_THIS; }
		const T& getConst() const{ return *mRef; }

		//---
		void save(cByteBuffer* pSaveHere) { DONT_USE_THIS; }
		void loadEat(cByteBuffer* pChewToy, dReloadMap *aReloads = NULL) { DONT_USE_THIS; }

		#ifdef GT_THREADS
			void updateStart(){ DONT_USE_THIS; }
			void updateFinish(){ DONT_USE_THIS; }
		#endif

	protected:
	#		ifdef GT_THREADS
			void readShadow(cBase_plug *pWriteTo, dConSig aCon){ DONT_USE_THIS; }
			void writeShadow(const cBase_plug *pReadFrom, dConSig aCon){ DONT_USE_THIS; }
			virtual void shadowAppends(cBase_plug *pWriteTo, dConSig pSig){ DONT_USE_THIS; }
			virtual void appendShadow(cBase_plug *pReadFrom, dConSig pSig){ DONT_USE_THIS; }
	#		endif

	private:
		const T *mRef;

		tLitePlugConst(T *aRef){}
	};

	//----------------------------------------------------------------------------------------------------------------
	//!\brief	Allows for easy specialisation.
	template<
		typename ELEM_T,
		template<typename, typename> class CONT_T
	>
	class tGetter{
	public:
		typedef CONT_T< ELEM_T, std::allocator<ELEM_T> > dContainer;

		static
		ELEM_T * get(dContainer *from, size_t idx)
		{ DONT_USE_THIS; return NULL; }
	};

	//----------------------------------------------------------------------------------------------------------------
	template<
		typename ELEM_T,
		template<typename, typename> class CONT_T
	>
	class tLitePlugLinearContainer : public cBase_plugContainer{
	public:
		//--- types
		typedef CONT_T< ELEM_T, std::allocator<ELEM_T> > dContainer;
		typedef tCoolItr< dContainer > dItr;

		//---
		tLitePlugLinearContainer(const tLitePlugLinearContainer &copyMe);
		tLitePlugLinearContainer(dContainer *refMe);
		virtual ~tLitePlugLinearContainer();

		void assignTo(void *pTo, dPlugType pType) const;
		void appendTo(void *pTo, dPlugType pType) const;
		size_t getCount() const;
		cBase_plug* getPlug(size_t idx);
		const cBase_plug* getPlugConst(size_t idx) const;
		void add(const cBase_plug &addMe);
		void clear();

		bool operator== (const cBase_plug &pD) const;
		cBase_plug& operator= (const cBase_plug &pD);
		cBase_plug& operator+= (const cBase_plug &pD);
		cBase_plugContainer& operator= (const cBase_plugContainer &pCopyMe);
		cBase_plugContainer& operator+= (const cBase_plugContainer &pCopyMe);


		void save(cByteBuffer* pSaveHere) { DONT_USE_THIS; }
		void loadEat(cByteBuffer* pChewToy, dReloadMap *aReloads = NULL) { DONT_USE_THIS; }

#		ifdef GT_THREADS
			virtual void updateStart() { DONT_USE_THIS; }
			virtual void updateFinish() { DONT_USE_THIS; }
#		endif

	protected:
		typedef void (*fuAssign)(const ELEM_T *copyFrom, void *copyTo);
		typedef void (*fuAppend)(const ELEM_T *copyFrom, void *copyTo);
		typedef std::map<dPlugType, fuAssign> dMapAssigns;
		typedef std::map<dPlugType, fuAppend> dMapAppends;

#		ifdef GT_THREADS
			virtual void shadowAppends(cBase_plug *pWriteTo, dConSig pSig) { DONT_USE_THIS; }
			virtual void appendShadow(cBase_plug *pReadFrom, dConSig pSig) { DONT_USE_THIS; }
			virtual void readShadow(cBase_plug *pWriteTo, dConSig pCon) { DONT_USE_THIS; }
			virtual void writeShadow(const cBase_plug *pReadFrom, dConSig pCon) { DONT_USE_THIS; }
#		endif

	private:
		tLitePlug<ELEM_T> *tmpRtnPlug;
		dContainer *mContainer;

		void internalFrom(cBase_plug &pD, bool pClear);
		void internalTo(void *pTo, dPlugType pType, bool pClear) const;
	};
}

///////////////////////////////////////////////////////////////////////////////////
// Template implementation
namespace gt{

	//----------------------------------------------------------------------------------------------------------------
	template< typename ELEM_T, template<typename, typename> class CONT_T >
	tLitePlugLinearContainer<ELEM_T, CONT_T>::tLitePlugLinearContainer(dContainer *refMe)
	: tmpRtnPlug(NULL), mContainer(refMe)
	{}

	template< typename ELEM_T, template<typename, typename> class CONT_T >
	tLitePlugLinearContainer<ELEM_T, CONT_T>::tLitePlugLinearContainer(const tLitePlugLinearContainer<ELEM_T, CONT_T> &copyMe)
	: tmpRtnPlug(NULL), mContainer(const_cast<dContainer*>(copyMe.mContainer))
	{}

	template< typename ELEM_T, template<typename, typename> class CONT_T >
	tLitePlugLinearContainer<ELEM_T, CONT_T>::~tLitePlugLinearContainer(){
		SAFEDEL(tmpRtnPlug);
	}

	template< typename ELEM_T, template<typename, typename> class CONT_T >
	void
	tLitePlugLinearContainer<ELEM_T, CONT_T>::assignTo(void *pTo, dPlugType pType) const{
		internalTo(pTo, pType, true);
	}

	template< typename ELEM_T, template<typename, typename> class CONT_T >
	void
	tLitePlugLinearContainer<ELEM_T, CONT_T>::appendTo(void *pTo, dPlugType pType) const{
		internalTo(pTo, pType, false);
	}

	template< typename ELEM_T, template<typename, typename> class CONT_T >
	size_t
	tLitePlugLinearContainer<ELEM_T, CONT_T>::getCount() const{
		return mContainer->size();
	}

	template< typename ELEM_T, template<typename, typename> class CONT_T >
	cBase_plug*
	tLitePlugLinearContainer<ELEM_T, CONT_T>::getPlug(size_t idx){
		SAFEDEL(tmpRtnPlug);
		tmpRtnPlug = new tLitePlug<ELEM_T>(
			tGetter<ELEM_T, CONT_T>::get(mContainer, idx)
		);
		return tmpRtnPlug;
	}

	template< typename ELEM_T, template<typename, typename> class CONT_T >
	const cBase_plug*
	tLitePlugLinearContainer<ELEM_T, CONT_T>::getPlugConst(size_t idx) const{
		return const_cast<
			tLitePlugLinearContainer<ELEM_T, CONT_T>*
		>(this)->getPlug(idx);
	}

	template< typename ELEM_T, template<typename, typename> class CONT_T >
	void
	tLitePlugLinearContainer<ELEM_T, CONT_T>::add(const cBase_plug &addMe){
		internalFrom( const_cast<cBase_plug&>(addMe), false );
	}

	template< typename ELEM_T, template<typename, typename> class CONT_T >
	void
	tLitePlugLinearContainer<ELEM_T, CONT_T>::clear(){
		mContainer->clear();
	}

	template< typename ELEM_T, template<typename, typename> class CONT_T >
	bool
	tLitePlugLinearContainer<ELEM_T, CONT_T>::operator== (const cBase_plug &pD) const{
		return (getType() == pD.getType());
	}

	template< typename ELEM_T, template<typename, typename> class CONT_T >
	cBase_plug&
	tLitePlugLinearContainer<ELEM_T, CONT_T>::operator= (const cBase_plug &pD){
		NOTSELF(&pD);
		internalFrom( const_cast<cBase_plug&>(pD), true );
		return *this;
	}

	template< typename ELEM_T, template<typename, typename> class CONT_T >
	cBase_plug&
	tLitePlugLinearContainer<ELEM_T, CONT_T>::operator+= (const cBase_plug &pD){
		NOTSELF(&pD);
		internalFrom( const_cast<cBase_plug&>(pD), false );
		return *this;
	}

	template< typename ELEM_T, template<typename, typename> class CONT_T >
	cBase_plugContainer&
	tLitePlugLinearContainer<ELEM_T, CONT_T>::operator= (const cBase_plugContainer &pCopyMe){
		NOTSELF(&pCopyMe);
		internalFrom(
			const_cast<cBase_plug&>(
				static_cast<const cBase_plug&>(	pCopyMe )
			),
			true
		);
		return *this;
	}

	template< typename ELEM_T, template<typename, typename> class CONT_T >
	cBase_plugContainer&
	tLitePlugLinearContainer<ELEM_T, CONT_T>::operator+= (const cBase_plugContainer &pCopyMe){
		NOTSELF(&pCopyMe);
		internalFrom(
			const_cast<cBase_plug&>(
				static_cast<const cBase_plug&>(	pCopyMe )
			),
			false
		);
		return *this;
	}


	template< typename ELEM_T, template<typename, typename> class CONT_T >
	void
	tLitePlugLinearContainer<ELEM_T, CONT_T>::internalFrom(cBase_plug &pD, bool pClear){
		PROFILE;

		if(pClear)
			mContainer->clear();

		dPlugType elemType = genPlugType<ELEM_T>();
		ELEM_T elemTmp;

		if(pD.getType() == getType()){
			cBase_plugContainer &tmp = dynamic_cast<cBase_plugContainer&>(pD);

			for(size_t i=0; i < tmp.getCount(); ++i){
				tmp.getPlug(i)->assignTo(&elemTmp, elemType);
				mContainer->push_back(elemTmp);
			}

		}else if(pD.getType() == elemType){
			pD.assignTo(&elemTmp, elemType);
			mContainer->push_back(elemTmp);
		}
	}

	template< typename ELEM_T, template<typename, typename> class CONT_T >
	void
	tLitePlugLinearContainer<ELEM_T, CONT_T>::internalTo(void *pTo, dPlugType pType, bool pClear) const{
		PROFILE;

		if(mContainer->empty())
			return;

		if(pType == getType()){
			cBase_plugContainer *tmp = static_cast<cBase_plugContainer*>(pTo);

			if(pClear)
				tmp->clear();

			for(typename dContainer::iterator itr = mContainer->begin(); itr != mContainer->end(); ++itr)
				tmp->add( tLitePlug<ELEM_T>( &(*itr) ) );

		}else{
			tLitePlug<ELEM_T> tmp(
				& const_cast<dContainer*>(mContainer)->front()
			);
			tmp.assignTo(pTo, pType);
		}
	}
}

///////////////////////////////////////////////////////////////////////////////////
// specialisations.
namespace gt{

	template<typename ELEM_T>
	class tGetter<ELEM_T, std::vector>{
	public:
		typedef std::vector< ELEM_T, std::allocator<ELEM_T> > dContainer;

		static
		ELEM_T * get(dContainer *from, size_t idx){
			ASRT_INRANGE( (*from), idx);
			return &from->at(idx);
		}
	};

	template<typename ELEM_T>
	class tGetter<ELEM_T, std::list>{
	public:
		typedef std::list< ELEM_T, std::allocator<ELEM_T> > dContainer;

		static
		ELEM_T * get(dContainer *from, size_t idx){
			ASRT_INRANGE( (*from), idx);
			typename dContainer::iterator itr( from->begin() );
			for(size_t i=0; i < idx; ++i)
				++itr;

			return &(*itr);
		}
	};

}

#endif
