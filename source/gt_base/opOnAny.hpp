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
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
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
#include "utils.hpp"
#include <vector>
#include <list>
#include <map>

#ifdef USE_TYPEINFO
#	include <typeinfo>
#endif

namespace gt{


	///////////////////////////////////////////////////////////////////////////////////
	typedef dNameHash dPlugType;

	///////////////////////////////////////////////////////////////////////////////////
	template<typename PLUG_TYPE> dPlugType genPlugType();	//!<

	///////////////////////////////////////////////////////////////////////////////////

	//-------------------------------------------------------------------------------------
	//!\breif	Any-op allows you to expand, at runtime, the ways in which a data type can
	//!			be assigned or appended. This is perfect for dynamic libraries which can
	//!			contain new types.
	class cAnyOp{
	public:

		//--------------------------------------------------------------------------------
		//!\brief	The catalogue of operations for a given type. Finish him! But serious,
		//!			the K is to help differentiate it from the typical shorthand for
		//!			concatenate.
		//!			The catalogue interface is here to let any-op add and remove them as
		//!			different dynamic libraries open and close.
		//!\note	Function pointers ARE NOT the same as pointers to data and must be
		//!			treated that way (even thought at the end of the day, both are memory
		//!			addresses). You can cast function pointers which is about as smelly
		//!			as casting void*. Everyone does either/or (win32, glib), so let's not
		//!			whine too hard. Here we're going with a hybrid to try and keep as
		//!			information about the parameters as possible.
		class iKat{
		public:
			virtual ~iKat(){}

			virtual dPlugType getType() =0;
			virtual void unlink(cAnyOp * pFrom) =0;	//!< Remove any functions that came from this any-op.
		};

		//--------------------------------------------------------------------------------
		//!\brief	By using this template class and calling these functions statically,
		//!			we ensure a kat exists for every type we use.
		//!\note	Public type because the seupKat function below must be pubic.
		template<typename A>
		class tKat : public iKat{
		public:

			//--- types
			typedef void (*fuAssign)(const A & pFrom, void * pTo);
			typedef void (*fuAppend)(const A & pFrom, void * pTo);
			typedef std::map<dPlugType, fuAssign> dMapAssigns;
			typedef std::map<dPlugType, fuAppend> dMapAppends;

			//---
			void addAss(cAnyOp * pFrom, dPlugType pFor, fuAssign pFu);
			void addApp(cAnyOp * pFrom, dPlugType pFor, fuAppend pFu);

			//---
			dPlugType getType();
			void unlink(cAnyOp * pFrom);

			//---
			static dMapAssigns & assign();
			static dMapAppends & append();

		protected:
			~tKat();	//!<

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
			dOp2Link mLinks;	//!< What any-ops are linked and where those functions came from them.

			tKat();	//!< Only the static should be created per heap.

			static tKat xKat;	//!< If your code uses any of the public functions, this should cause the compiler to create an instance of a catalogue for this type.
		};

		//--------------------------------------------------------------------------------
		//!\breif	It's much easier to specialise a class than a function which would
		//!			need a declaration in the header and a definition in the source. It
		//!			also encourages modularity for the functions you will want to add.
		//!\note	http://www.gotw.ca/publications/mill17.htm
		template<typename A>
		class tOps{
		public:
			static void setup(tKat<A> * pK, cAnyOp * pUsing);
		};

		//---
		template<typename A>
		static void setupKat(tKat<A> * pK);	//!< Public so that tKat can make use of it.

		template<typename A>
		static void assign(const A & pFrom, void * pTo, dPlugType pType);

		template<typename A>
		static void append(const A & pFrom, void * pTo, dPlugType pType);

		template<typename A>
		static void fuAssignDefault(const A & pFrom, void * pTo);

		template<typename A>
		static void fuAppendDefault(const A & pFrom, void * pTo);

	protected:

		//---
		void merge(cAnyOp * pOther);	//!< Mutual merge. Remembers the other any-op, so when you call clear the 2 can be unlinked again.
		void demerge();	//!<

		//---
		static cAnyOp& getRef();

		friend class cWorld;

	private:

		typedef std::map<dPlugType, iKat*> dKats;

		//---
		dKats mKats;	//!< Only storing a kat for each type.
		std::list<cAnyOp*> mLinks;	//!< Ensure Kats inside other any-ops know their links from here are no longer good.

		//---
		cAnyOp();	//!<
		~cAnyOp();	//!< Lets any remaining linked any-ops know it died.

		//---

	};

}

///////////////////////////////////////////////////////////////////////////////////
namespace gt{

	//--------------------------------------------------------------------------------
	template<typename A>
	cAnyOp::tKat<A> cAnyOp::tKat<A>::xKat;
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

	template<typename A>
	cAnyOp::tKat<A>::tKat()
	{
		cAnyOp::setupKat<A>(this);
	}

	template<typename A>
	cAnyOp::tKat<A>::~tKat(){
	}

	template<typename A>
	typename cAnyOp::tKat<A>::dMapAssigns &
	cAnyOp::tKat<A>::assign(){
		return xKat.mAsss;
	}

	template<typename A>
	typename cAnyOp::tKat<A>::dMapAppends &
	cAnyOp::tKat<A>::append(){
		return xKat.mApps;
	}

	template<typename A>
	dPlugType
	cAnyOp::tKat<A>::getType(){
		return genPlugType<A>();
	}

	template<typename A>
	void
	cAnyOp::tKat<A>::unlink(cAnyOp * pFrom){
		typename dOp2Link::iterator found = mLinks.find(pFrom);
		if(found == mLinks.end())
			return;

		dListAssItr & listAss = found->second.mAsss;
		typename dListAssItr::iterator itrAss;
		for(itrAss = listAss.begin(); itrAss != listAss.end(); ++itrAss)
			mAsss.erase(*itrAss);

		dListAppItr & listApp = found->second.mApps;
		typename dListAppItr::iterator itrApp;
		for(itrApp = listApp.begin(); itrApp != listApp.end(); ++itrApp)
			mApps.erase(*itrApp);
	}

	template<typename A>
	void
	cAnyOp::tKat<A>::addAss(cAnyOp * pFrom, dPlugType pFor, fuAssign pFu){
		std::pair<typename dMapAssigns::iterator, bool> result = mAsss.insert(
			typename dMapAssigns::value_type(pFor, pFu)
		);

		if(!result.second)
			return;

		typename dOp2Link::iterator link = mLinks.insert(
			mLinks.end(),
			typename dOp2Link::value_type(pFrom, sLinkOp())
		);
		link->second.mAsss.push_back(result.first);
	}

	template<typename A>
	void
	cAnyOp::tKat<A>::addApp(cAnyOp * pFrom, dPlugType pFor, fuAppend pFu){
		std::pair<typename dMapAppends::iterator, bool> result = mAsss.insert(
			typename dMapAppends::value_type(pFor, pFu)
		);

		if(!result.second)
			return;

		typename dOp2Link::iterator link = mLinks.insert(
			mLinks.end(),
			typename dOp2Link::value_type(pFrom, sLinkOp())
		);
		link->second.mApps.push_back(result.first);
	}

	//-------------------------------------------------------------------------------
	template<typename A>
	void cAnyOp::tOps<A>::setup(tKat<A> * pK, cAnyOp * pUsing){
		pK->addAss(&getRef(), genPlugType<A>(), fuAssignDefault<A>);
		pK->addApp(&getRef(), genPlugType<A>(), fuAppendDefault<A>);
	}

	//-------------------------------------------------------------------------------

	//- This is the default setup that can only operate between the same types
	template<typename A>
	void
	cAnyOp::setupKat(tKat<A> * pK){
		tOps<A>::setup(pK, &getRef());
		(void)getRef().mKats.insert( dKats::value_type(pK->getType(), pK) );
	}

	template<typename A>
	void
	cAnyOp::assign(const A & pFrom, void * pTo, dPlugType pType){
		tCoolFind<dPlugType, typename cAnyOp::tKat<A>::fuAssign> op(
			cAnyOp::tKat<A>::assign(),
			pType
		);

		if(!op.found())
			THROW_ERROR(genPlugType<A>() << " can't assign to type " << pType);

		op.get()(pFrom, pTo);
	}

	template<typename A>
	void
	cAnyOp::append(const A & pFrom, void * pTo, dPlugType pType){
		tCoolFind<dPlugType, typename cAnyOp::tKat<A>::fuAssign> op(
			cAnyOp::tKat<A>::append(),
			pType
		);

		if(!op.found())
			THROW_ERROR(genPlugType<A>() << " can't append to type " << pType);

		op.get()(pFrom, pTo);
	}

	template<typename A>
	void
	cAnyOp::fuAssignDefault(const A & pFrom, void *pTo){
		*static_cast<A*>(pTo) = pFrom;
	}

	template<typename A>
	void
	cAnyOp::fuAppendDefault(const A & pFrom, void *pTo){
		*static_cast<A*>(pTo) += pFrom;
	}
}



#endif
