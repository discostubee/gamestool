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

#ifndef	BASEPLUG_HPP
#define BASEPLUG_HPP

///////////////////////////////////////////////////////////////////////////////////
// Config
//!\brief	Enable or disable typeID as the method used to identify plug types.
#define USE_TYPEINFO

///////////////////////////////////////////////////////////////////////////////////
// Includes

#include "context.hpp"

#ifdef USE_TYPEINFO
#	include <typeinfo>
#endif


///////////////////////////////////////////////////////////////////////////////////
namespace gt{

	//----------------------------------------------------------------------------------------------------------------
	//!\brief	Void assigns let us map different copying techniques to each type, for each type. If the target wasn't void, we wouldn't be
	//!			able to map them as function pointers.
	namespace voidAssign{

		//!\brief Try a static cast conversion.
		template<typename A> void basic(const A *pFrom, void *pTo){
			*static_cast<A*>(pTo) = *pFrom;
		}
	}

	//----------------------------------------------------------------------------------------------------------------
	//!\brief
	namespace voidAppend{

		//!\brief Try a static cast conversion.
		template<typename A> void basic(const A *pFrom, void *pTo){
			*static_cast<A*>(pTo) += *pFrom;
		}
	}
}

///////////////////////////////////////////////////////////////////////////////////
// Classes
namespace gt{

	//----------------------------------------------------------------------------------------------------------------
	//!\brief	A plug is a data container that a lead can connect too. The lead can then connect that data to another
	//!			object via the jack function, as well as automatically disconnect itself from its linked leads when it
	//!			dies.
	//!\note	You don't have to use plugs for all your figments stuff, Just for the things you want to save and
	//!			reload or pass through a lead to another object.
	//!\note	Using assignment operators doesn't use the other plugs shadows. This is because shadows are only for
	//!			leads, and shadows are meant to solve the issue of data access over different threads. Inter-thread
	//!			access should only happen through leads.
	class cBase_plug{
	public:
		//--- types
		typedef dNameHash dPlugType;

		//--- implemented
		cBase_plug();
		virtual ~cBase_plug();

		template<typename PLUG_TYPE> static dPlugType genPlugType();	//!<

		virtual void linkLead(cLead* pLead); //!< Add a new link, or increase the number of times this lead is linked to this plug.	!\note	Made threadsafe in implementation.
		virtual void unlinkLead(cLead* pLead); //!< Decrements the number of links, only disconnecting when there is 0 links to this lead. !\note	Made threadsafe in implementation.

		//--- interface
		virtual dPlugType getType() const =0;
		virtual void assignTo(void *pTo, dPlugType pType) const =0;	//!< Allows later implementation to assign into the memory address. !\note Not the same as copying because things like smart pointers should work correctly with this.
		virtual void appendTo(void *pTo, dPlugType pType) const =0;	//!< Similar to assign, but for appending to the input argument.
		virtual void save(cByteBuffer* pSaveHere) =0;	//!< Appends the buffer with binary data that should be understandable by any platform.
		virtual void loadEat(cByteBuffer* pChewToy, dReloadMap *aReloads = NULL) =0;	//!< Reloads data from the buffer and deletes the contents it used (because save or loading is a one to one operation).

		virtual bool operator== (const cBase_plug &pD) const =0;
		virtual	cBase_plug& operator= (const cBase_plug &pD) =0;	//!< Assigns only the content, should not copy any linked lead info.
		virtual cBase_plug& operator+= (const cBase_plug &pD) =0;

		#ifdef GT_THREADS
			virtual void updateStart() =0;	//!< Write shadow updates to origin.
			virtual void updateFinish() =0;	//!< Update all shadows using the origin. Any shadow writes after the update was started are overwritten.
		#endif


	protected:
		typedef std::map<cLead*, unsigned int> dMapLeads;

		dMapLeads mLeadsConnected;		//!< Lead connections are not copied when copy plug values.

		#ifdef GT_THREADS
			virtual void readShadow(cBase_plug *pWriteTo, dConSig pSig) =0;
			virtual void writeShadow(const cBase_plug *pReadFrom, dConSig pSig) =0;
			virtual void shadowAppends(cBase_plug *pWriteTo, dConSig pSig) =0;
			virtual void appendShadow(cBase_plug *pReadFrom, dConSig pSig) =0;
		#endif

	friend class cLead;
	friend class cBasePlugLinierContainer;

	private:
		cBase_plug(const cBase_plug &pCopy);
	};

	//----------------------------------------------------------------------------------------------------------------
	//!\brief	Used by containers to appear the same no matter what the underlying container type is.
	class cBasePlugContainer : public cBase_plug {
	public:
		cBasePlugContainer();
		virtual ~cBasePlugContainer();

		virtual dPlugType getType() const;	//!< Any type of linear container should see any other linear

		//- Would like to protect these, but can't.
		virtual size_t getCount() const =0;
		virtual cBase_plug* getPlug(size_t idx) =0;
		virtual const cBase_plug* getPlugConst(size_t idx) const =0;
		virtual void add(const cBase_plug &addMe) =0;
		virtual void clear() =0;

	};

	//----------------------------------------------------------------------------------------------------------------
	//!\brief	Another step towards a full plug, designed just to manage assignments and appends.
	//!\note	Putting the assignments and append maps in here so that it's easier to specialise
	template<typename A>
	class tDataPlug: public cBase_plug{
	public:
		//--- types
		typedef void (*fuAssign)(const A *copyFrom, void *copyTo);
		typedef void (*fuAppend)(const A *copyFrom, void *copyTo);
		typedef std::map<dPlugType, fuAssign> dMapAssigns;
		typedef std::map<dPlugType, fuAppend> dMapAppends;

		//--- implemented
		virtual cBase_plug::dPlugType getType() const;
		virtual void assignTo(void *pTo, dPlugType pType) const;
		virtual void appendTo(void *pTo, dPlugType pType) const;

		virtual bool operator== (const cBase_plug &pD) const;
		virtual cBase_plug& operator= (const cBase_plug &pD);
		virtual cBase_plug& operator+= (const cBase_plug &pD);

		//--- new interface
		virtual A& get() =0;
		virtual const A& getConst() const =0;

	private:
		tDataPlug<A>& operator= (const tDataPlug<A> &dontcare){ return *this; }
		tDataPlug<A>& operator+= (const tDataPlug<A> &dontcare){ return *this; }
	};

	//----------------------------------------------------------------------------------------------------------------
	//!\brief	Specialise the class for each type you want to have more than just a basic copy for.
	template<typename A>
	class tOpOnAny{
	public:

		//!\brief
		static typename tDataPlug<A>::dMapAssigns* assign(){
			static bool setup = false;
			static typename tDataPlug<A>::dMapAssigns ass;

			if(!setup){
				ass[ cBase_plug::genPlugType<A>() ] = voidAssign::basic<A>;
				setup=true;
			}

			return &ass;
		}

		//!\brief
		static typename tDataPlug<A>::dMapAppends* append(){
			static bool setup = false;
			static typename tDataPlug<A>::dMapAppends app;

			if(!setup){
				app[ cBase_plug::genPlugType<A>() ] = voidAppend::basic<A>;
				setup=true;
			}

			return &app;
		}

	private:
		tOpOnAny(){}
		~tOpOnAny(){}
	};

}

///////////////////////////////////////////////////////////////////////////////////
// Template implementation
namespace gt{

	//----------------------------------------------------------------------------------------------------------------
	template<typename PLUG_TYPE>
	cBase_plug::dPlugType
	cBase_plug::genPlugType(){

		static const dPlugType typeID
#			ifdef USE_TYPEINFO
				= makeHash(typeid(PLUG_TYPE).name());
#			else
				//- error for now
#			endif

		return typeID;
	}

	//----------------------------------------------------------------------------------------------------------------
	template<typename A>
	cBase_plug::dPlugType
	tDataPlug<A>::getType() const {
		return cBase_plug::genPlugType<A>();
	}

	template<typename A>
	void
	tDataPlug<A>::assignTo(void *pTo, cBase_plug::dPlugType pType) const{
		PROFILE;

		tCoolFind<cBase_plug::dPlugType, fuAssign> op(
			*tOpOnAny<A>::assign(),
			pType
		);

		if(!op.found())
			throw excep::cantCopy(typeid(A).name(), "unknown type", __FILE__, __LINE__);

		op.get()(
			&getConst(),
			pTo
		);
	}

	template<typename A>
	void
	tDataPlug<A>::appendTo(void *pTo, cBase_plug::dPlugType pType) const{
		PROFILE;

		tCoolFind<cBase_plug::dPlugType, fuAssign> op(
			*tOpOnAny<A>::append(),
			pType
		);

		if(!op.found())
			throw excep::cantCopy(typeid(A).name(), "unknown type", __FILE__, __LINE__);

		op.get()(
			&getConst(),
			pTo
		);
	}

	template<typename A>
	bool
	tDataPlug<A>::operator== (const cBase_plug &pD) const {
		return (cBase_plug::genPlugType<A>() == pD.getType());
	}

	template<typename A>
	cBase_plug&
	tDataPlug<A>::operator= (const cBase_plug &pD){
		NOTSELF(&pD);
		pD.assignTo(&get(), cBase_plug::genPlugType<A>());
		return *this;
	}

	template<typename A>
	cBase_plug&
	tDataPlug<A>::operator+= (const cBase_plug &pD){
		NOTSELF(&pD);
		pD.appendTo(&get(), cBase_plug::genPlugType<A>());
		return *this;
	}


}


#endif
