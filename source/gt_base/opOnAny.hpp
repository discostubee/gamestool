/*
 * !\file	opOnAny.hpp
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
 *  MERCHANTABILITY or FITNESS FOR ELEM_T PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received ELEM_T copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *********************************************************************************************************
 *
 */

#ifndef	OPONANY_HPP
#define OPONANY_HPP

///////////////////////////////////////////////////////////////////////////////////
// Config
//!\brief	Enable or disable typeID as the method used to identify plug types.
#define USE_TYPEINFO

///////////////////////////////////////////////////////////////////////////////////
//
#include "utils.hpp"
#include "exceptions.hpp"

#include <vector>
#include <list>
#include <map>

#ifdef USE_TYPEINFO
#	include <typeinfo>
#endif

///////////////////////////////////////////////////////////////////////////////////
// types
namespace gt{
	typedef dNameHash dPlugType;
	template<typename PLUG_TYPE> dPlugType genPlugType();	//!<
}

///////////////////////////////////////////////////////////////////////////////////
// objects
namespace gt{


	//-------------------------------------------------------------------------------------
	//!\breif	Any-op allows you to expand, at runtime, the ways in which ELEM_T data type can
	//!			be assigned or appended. This is perfect for dynamic libraries which can
	//!			contain new types.
	//!\note	There should be no need to make this threadsafe, because Any-ops are manipulated
	//!			only when a library opens or closes, which is made threadsafe by gWorld.
	class cAnyOp{
	public:

		//--------------------------------------------------------------------------------
		//!\brief	The catalogue of operations for ELEM_T given type. Finish him! But serious,
		//!			the K is to help differentiate it from the typical shorthand for
		//!			concatenate.
		//!			The catalogue interface is here to let any-op add and remove them as
		//!			different dynamic libraries open and close.
		//!\note	Function pointers ARE NOT the same as pointers to data and must be
		//!			treated that way (even thought at the end of the day, both are memory
		//!			addresses). You can cast function pointers which is about as smelly
		//!			as casting void*. Everyone does either/or (win32, glib), so let's not
		//!			whine too hard. Here we're going with ELEM_T hybrid to try and keep as much
		//!			information about the parameters as possible.
		class iKat{
		public:
			virtual ~iKat(){}

			virtual void setup(cAnyOp * ops) =0;
			virtual dPlugType getType() const =0;
			virtual const dPlaChar * getName() const =0;
			virtual void link(cAnyOp * pFrom, iKat * pOps) =0;
			virtual void unlink(cAnyOp * pFrom) =0;	//!< Remove any operations that came from this Any-op collection.
		};

		//--------------------------------------------------------------------------------
		//!\brief	By using this template class and calling these functions statically,
		//!			we ensure ELEM_T kat exists for every type we use.
		//!\note	Public type because the setupKat function below must be pubic.
		template<typename ELEM_T>
		class tKat : public iKat{
		public:

			//--- types
			typedef void (*fuAssign)(const ELEM_T & pFrom, void * pTo);
			typedef void (*fuAppend)(const ELEM_T & pFrom, void * pTo);
			typedef std::map<dPlugType, fuAssign> dMapAssigns;
			typedef std::map<dPlugType, fuAppend> dMapAppends;

			//---
			bool addAss(cAnyOp * pFrom, dPlugType pFor, fuAssign pFu);	//!< True if this assignment operation was added. False if it already that that operation.
			bool addApp(cAnyOp * pFrom, dPlugType pFor, fuAppend pFu);	//!< True if this append operation was added. False if it already that that operation.

			//---
			void setup(cAnyOp * ops);
			dPlugType getType() const;
			const dPlaChar * getName() const;
			void link(cAnyOp * pFrom, iKat * pOps);
			void unlink(cAnyOp * pFrom);	//!< Not an error if it doesn't have that Any-op linked.

			//---
			static dMapAssigns & assign();
			static dMapAppends & append();

			//---
			static tKat xKat;	//!< If you create ELEM_T specialisation of tOps, this should cause the compiler to create an instance of ELEM_T catalogue for this type. Public, so that other types can access it.

		protected:
			~tKat();

		private:
			typedef std::list<typename dMapAssigns::iterator> dListAssItr;
			typedef std::list<typename dMapAppends::iterator> dListAppItr;

			struct sLinkOp{
				dListAssItr mAsss;
				dListAppItr mApps;
			};

			typedef std::map<cAnyOp*, sLinkOp> dOp2Link;

			dMapAssigns mAsss;
			dMapAppends mApps;
			dOp2Link mLinks;	//!< What Any-ops are linked and where those functions came from them.

			tKat();	//!< Only the static should be created per heap.
		};

		//--------------------------------------------------------------------------------
		//!\breif	It's much easier to specialise ELEM_T class than ELEM_T function which would
		//!			need ELEM_T declaration in the header and ELEM_T definition in the source. It
		//!			also encourages modularity for the functions you will want to add.
		//!\note	http://www.gotw.ca/publications/mill17.htm
		template<typename ELEM_T>
		class tOps{
		public:
			static void setup(tKat<ELEM_T> * pK, cAnyOp * pUsing);
		};

		//---
		static void addKat(iKat * pK);	//!< Makes it easy to modularise the kats.

		template<typename ELEM_T>
		static void assign(const ELEM_T & pFrom, void * pTo, dPlugType pType);

		template<typename ELEM_T>
		static void append(const ELEM_T & pFrom, void * pTo, dPlugType pType);

		template<typename ELEM_T>
		static void fuAssignDefault(const ELEM_T & pFrom, void * pTo);

		template<typename ELEM_T>
		static void fuAppendDefault(const ELEM_T & pFrom, void * pTo);

	protected:

		//---
		void setupAll();
		void merge(cAnyOp * pOther);	//!< Mutual merge. Remembers the other Any-op, so when you call clear the 2 can be unlinked again.
		void merge(cAnyOp *me, cAnyOp *you);
		void demerge();	//!< Unlinks all other Any-ops.
		void demerge(cAnyOp *pOther);
		void demerge(cAnyOp *me, cAnyOp *you); //!< Unlinks the given Any-op from this one.

		//---
		static cAnyOp& getRef();

		friend class cWorld;

	private:
		struct sKatOrig{
			iKat * const mRefKat;	//!< Don't clean up.
			cAnyOp * const mOrig; //!< Is NULL if the origin of this catalogue is this heap/any-op.

			sKatOrig(iKat *pKat) : mRefKat(pKat), mOrig(NULL) {}
			sKatOrig(iKat *pKat, cAnyOp *pOrig) : mRefKat(pKat), mOrig(pOrig) {}
		};

		typedef std::map<dPlugType, sKatOrig> dMapKatTypes;

		//---
		dMapKatTypes mKats;
		std::list<cAnyOp*> mLinkedAnyOps;	//!< Remember the other ops we linked to.

		//---
		cAnyOp();	//!<
		~cAnyOp();	//!< Lets any remaining linked any-ops know it died.

		//---
		static void merge(dMapKatTypes *myKats, dMapKatTypes *yourKats, cAnyOp *me);
	};

}

///////////////////////////////////////////////////////////////////////////////////
namespace gt{

	//--------------------------------------------------------------------------------
	template<typename ELEM_T>
	cAnyOp::tKat<ELEM_T> cAnyOp::tKat<ELEM_T>::xKat;
}

///////////////////////////////////////////////////////////////////////////////////
namespace gt{


	//----------------------------------------------------------------------------------------------------------------
	template<typename PLUG_TYPE>
	dPlugType
	genPlugType(){
		static const dPlugType typeID
#			ifdef USE_TYPEINFO
				= makeHash(typeid(PLUG_TYPE).name());
#			else
				//- error for now
#			endif

		return typeID;
	}

	//----------------------------------------------------------------------------------------------------------------
	template<typename ELEM_T>
	cAnyOp::tKat<ELEM_T>::tKat(){
		cAnyOp::addKat(this);
	}

	template<typename ELEM_T>
	cAnyOp::tKat<ELEM_T>::~tKat(){
	}

	template<typename ELEM_T>
	typename cAnyOp::tKat<ELEM_T>::dMapAssigns &
	cAnyOp::tKat<ELEM_T>::assign(){
		return xKat.mAsss;
	}

	template<typename ELEM_T>
	typename cAnyOp::tKat<ELEM_T>::dMapAppends &
	cAnyOp::tKat<ELEM_T>::append(){
		return xKat.mApps;
	}

	template<typename ELEM_T>
	void
	cAnyOp::tKat<ELEM_T>::setup(cAnyOp * ops){
		tOps<ELEM_T>::setup(this, ops);
	}

	template<typename ELEM_T>
	dPlugType
	cAnyOp::tKat<ELEM_T>::getType() const{
		return genPlugType<ELEM_T>();
	}

	template<typename ELEM_T>
	const dPlaChar *
	cAnyOp::tKat<ELEM_T>::getName() const{
		return typeid(ELEM_T).name();
	}

	template<typename ELEM_T>
	void
	cAnyOp::tKat<ELEM_T>::link(cAnyOp * pFrom, iKat * pOps){
		ASRT_NOTNULL(pFrom);
		ASRT_NOTNULL(pOps);
		typename dOp2Link::iterator links = mLinks.find(pFrom);
		if(links != mLinks.end()){
			DBUG_LO("Already linked " << getName());
			return;
		}

		ASRT_TRUE(pOps->getType() == getType(), "Not the same types");
		tKat<ELEM_T> * you = dynamic_cast< tKat<ELEM_T>* >(pOps);
		ASRT_NOTNULL(you);

		links = mLinks.insert(
			mLinks.end(),
			typename dOp2Link::value_type(pFrom, sLinkOp())
		);
		DBUG_VERBOSE_LO("Linking " << getName() << " to " << std::hex << pFrom);

		typename dMapAssigns::iterator hasAss;
		for(
			typename dMapAssigns::iterator itrYourAss = you->mAsss.begin();
			itrYourAss != you->mAsss.end();
			++itrYourAss
		){
			hasAss = mAsss.find(itrYourAss->first);
			if(hasAss == you->mAsss.end()){
				hasAss = mAsss.insert(
					mAsss.end(),
					*itrYourAss
				);
				links->second.mAsss.push_back(hasAss);
				DBUG_VERBOSE_LO("   assignment op for " << itrYourAss->first);
			}
		}

		typename dMapAssigns::iterator hasApp;
		for(
			typename dMapAppends::iterator itrYourApp = you->mApps.begin();
			itrYourApp != you->mApps.end();
			++itrYourApp
		){
			hasApp = mApps.find(itrYourApp->first);
			if(hasApp == you->mApps.end()){
				hasApp = mApps.insert(
					mApps.end(),
					*itrYourApp
				);
				links->second.mApps.push_back(hasApp);
				DBUG_VERBOSE_LO("   assignment op for " << itrYourApp->first);
			}
		}
	}

	template<typename ELEM_T>
	void
	cAnyOp::tKat<ELEM_T>::unlink(cAnyOp * pFrom){
		ASRT_NOTNULL(pFrom);
		typename dOp2Link::iterator found = mLinks.find(pFrom);
		if(found == mLinks.end()){
			DBUG_LO(getName() << " not linked to " << std::hex << pFrom);
			return;
		}

		DBUG_VERBOSE_LO("Unlinking: " << getName());
		dListAssItr & listAss = found->second.mAsss;
		for(
			typename dListAssItr::iterator itrAss = listAss.begin();
			itrAss != listAss.end();
			++itrAss
		){
			mAsss.erase((*itrAss));
			DBUG_VERBOSE_LO( "   assignment op from " << (*itrAss)->first);
		}

		dListAppItr & listApp = found->second.mApps;
		for(
			typename dListAppItr::iterator itrApp = listApp.begin();
			itrApp != listApp.end();
			++itrApp
		){
			mApps.erase((*itrApp));
			DBUG_VERBOSE_LO( "   append op from " << (*itrApp)->first);
		}
		mLinks.erase(found);
	}

	template<typename ELEM_T>
	bool
	cAnyOp::tKat<ELEM_T>::addAss(cAnyOp * pFrom, dPlugType pFor, fuAssign pFu){
		ASRT_NOTNULL(pFrom);
		std::pair<typename dMapAssigns::iterator, bool> result = mAsss.insert(
			typename dMapAssigns::value_type(pFor, pFu)
		);

		if(!result.second){
			return false;
		}

		typename dOp2Link::iterator link = mLinks.insert(
			mLinks.end(),
			typename dOp2Link::value_type(pFrom, sLinkOp())
		);
		link->second.mAsss.push_back(result.first);

		DBUG_VERBOSE_LO("Assignment op added: " << getName() << " to " << pFor);
		return true;
	}

	template<typename ELEM_T>
	bool
	cAnyOp::tKat<ELEM_T>::addApp(cAnyOp * pFrom, dPlugType pFor, fuAppend pFu){
		std::pair<typename dMapAppends::iterator, bool> result = mApps.insert(
			typename dMapAppends::value_type(pFor, pFu)
		);

		if(!result.second){
			return false;
		}

		typename dOp2Link::iterator link = mLinks.insert(
			mLinks.end(),
			typename dOp2Link::value_type(pFrom, sLinkOp())
		);
		link->second.mApps.push_back(result.first);

		DBUG_VERBOSE_LO("Append op added: " << getType() << " to " << pFor);
		return true;
	}

	//-------------------------------------------------------------------------------
	template<typename ELEM_T>
	void cAnyOp::tOps<ELEM_T>::setup(tKat<ELEM_T> * pK, cAnyOp * pUsing){
		pK->addAss(&getRef(), genPlugType<ELEM_T>(), fuAssignDefault<ELEM_T>);
		pK->addApp(&getRef(), genPlugType<ELEM_T>(), fuAppendDefault<ELEM_T>);
	}

	//-------------------------------------------------------------------------------

	template<typename ELEM_T>
	void
	cAnyOp::assign(const ELEM_T & pFrom, void * pTo, dPlugType pType){
		tCoolFind<dPlugType, typename cAnyOp::tKat<ELEM_T>::fuAssign> op(
			cAnyOp::tKat<ELEM_T>::assign(),
			pType
		);

		if(!op.found())
			THROW_ERROR(genPlugType<ELEM_T>() << " can't assign to type " << pType);

		op.get()(pFrom, pTo);
	}

	template<typename ELEM_T>
	void
	cAnyOp::append(const ELEM_T & pFrom, void * pTo, dPlugType pType){
		tCoolFind<dPlugType, typename cAnyOp::tKat<ELEM_T>::fuAppend> op(
			cAnyOp::tKat<ELEM_T>::append(),
			pType
		);

		if(!op.found())
			THROW_ERROR(genPlugType<ELEM_T>() << " can't append to type " << pType);

		op.get()(pFrom, pTo);
	}

	template<typename ELEM_T>
	void
	cAnyOp::fuAssignDefault(const ELEM_T & pFrom, void *pTo){
		*static_cast<ELEM_T*>(pTo) = pFrom;
	}

	template<typename ELEM_T>
	void
	cAnyOp::fuAppendDefault(const ELEM_T & pFrom, void *pTo){
		*static_cast<ELEM_T*>(pTo) += pFrom;
	}
}



#endif
