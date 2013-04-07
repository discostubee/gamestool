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

#include "command.hpp"
#include "context.hpp"

#ifdef USE_TYPEINFO
#	include <typeinfo>
#endif


///////////////////////////////////////////////////////////////////////////////////
// fu
namespace gt{

	//!\brief	Void assigns let us map different copying techniques to each type, for each type. If the target wasn't void, we wouldn't be
	//!			able to map them as function pointers.
	namespace voidAssign{

		//!\brief Try a static cast conversion.
		template<typename A> void basic(const A *pFrom, void *pTo){
			*static_cast<A*>(pTo) = *pFrom;
		}
	}

	//!\brief
	namespace voidAppend{

		//!\brief Try a static cast conversion.
		template<typename A> void basic(const A *pFrom, void *pTo){
			*static_cast<A*>(pTo) += *pFrom;
		}
	}

	template<typename A> class tDataPlug;

	//!\brief	Specialise this for each type you want to have more than just a basic copy for.
	//!\note	Made as a function separate from the class to make it easier to write new assignments.
	template<typename A> typename tDataPlug<A>::dMapAssigns* getVoidAssignments();

	//!\brief	Similar to getVoidAssignments.
	template<typename A> typename tDataPlug<A>::dMapAppends* getVoidAppends();

}

///////////////////////////////////////////////////////////////////////////////////
// Classes
namespace gt{

	//----------------------------------------------------------------------------------------------------------------
	//!\brief	this helps to identify what each plug is on a lead.
	class cPlugTag{
	public:
		typedef unsigned int dUID;	//!< Unique ID.

		const dStr	mName;
		const dUID	mID;

		cPlugTag(
			const dPlaChar* pPlugName
		):
			mName(pPlugName),
			mID(
				makeHash( toNStr(
					mName.c_str()
				).t.c_str() )
			)
		{}

		~cPlugTag()
		{}

	private:
		cPlugTag& operator = (const cPlugTag&){ return *this; }
	};

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

		//--- statics
		template<typename PLUG_TYPE> static dPlugType genPlugType();	//!<

		//--- implemented
		cBase_plug();
		virtual ~cBase_plug();

		virtual void linkLead(cLead* pLead); //!< Add a new link, or increase the number of times this lead is linked to this plug.	!\note	Made threadsafe in implementation.
		virtual void unlinkLead(cLead* pLead); //!< Decrements the number of links, only disconnecting when there is 0 links to this lead. !\note	Made threadsafe in implementation.

		template<typename T> cBase_plug& operator= (const T &aFrom);	//!< Can only work on exactly equal types.

		//--- interface
		virtual dPlugType getType() const =0;
		virtual void save(cByteBuffer* pSaveHere) = 0;	//!< Appends the buffer with binary data that should be understandable by any platform.
		virtual void loadEat(cByteBuffer* pChewToy, dReloadMap *aReloads = NULL) = 0;	//!< Reloads data from the buffer and delets the contents it used (because save or loading is a one to one operation).
		virtual void assign(void *aTo, dPlugType aType) const =0;	//!< Allows later implementation to assign into the memory addess. !\note Not the same as copying because things like smart pointers should work correctly with this.
		virtual void append(void *aTo, dPlugType aType) const =0;	//!< Similar to assign, but for appending to the input argument.

		virtual	cBase_plug& operator= (const cBase_plug &pD) =0;	//!< Assigns only the content, should not copy any linked lead info.
		virtual bool operator== (const cBase_plug &pD) const =0;
		virtual cBase_plug& operator+= (const cBase_plug &pD) =0;

		#ifdef GT_THREADS
			virtual void updateStart() =0;	//!< Write shadow updates to origin.
			virtual void updateFinish() =0;	//!< Update all shadows using the origin. Any shadow writes after the update was started are overwritten.
		#endif


	protected:
		typedef std::map<cLead*, unsigned int> dMapLeads;

		dMapLeads mLeadsConnected;		//!< Lead connections are not copied when copy plug values.
		dMapLeads::iterator itrLead;	//!< handy.

		#ifdef GT_THREADS
			virtual void readShadow(cBase_plug *pWriteTo, dConSig aCon) =0;
			virtual void writeShadow(const cBase_plug *pReadFrom, dConSig aCon) =0;
			virtual void appendShadow(const cBase_plug *pReadFrom, dConSig aCon) =0;
		#endif

	friend class cLead;

	private:
		cBase_plug(const cBase_plug &pCopy);
	};

	//----------------------------------------------------------------------------------------------------------------
	//!\brief	Another step towards a full plug, designed just to manage assignments and appends.
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
		virtual void assign(void *aTo, dPlugType aType) const;
		virtual void append(void *aTo, dPlugType aType) const;

		virtual bool operator== (const cBase_plug &pD) const;

		//--- interface
		virtual A& get() = 0;
		virtual const A& getConst() const =0;
	};

	//----------------------------------------------------------------------------------------------------------------
	//!\brief	Used just for copying and appending.
	template<typename T>
	class tLitePlug: public tDataPlug<T>{
	public:
		T *mRef;

		tLitePlug(T *aRef) : mRef(aRef) {}
		virtual ~tLitePlug();

		virtual void save(cByteBuffer* pSaveHere) { DONT_USE_THIS; }
		virtual void loadEat(cByteBuffer* pChewToy, dReloadMap *aReloads = NULL) { DONT_USE_THIS; }

		virtual	cBase_plug& operator= (const cBase_plug &pD) {
			ASRT_NOTSELF(&pD);
			pD.assign(mRef, cBase_plug::genPlugType<T>());
			return *this;
		}

		virtual cBase_plug& operator+= (const cBase_plug &pD) {
			ASRT_NOTSELF(&pD);
			pD.append(mRef, cBase_plug::genPlugType<T>());
			return *this;
		}

		virtual T& get(){ return *mRef; }
		virtual const T& getConst() const{ return *mRef; }

		#ifdef GT_THREADS
			virtual void updateStart(){ DONT_USE_THIS; }
			virtual void updateFinish(){ DONT_USE_THIS; }
		#endif

	protected:
#		ifdef GT_THREADS
			virtual void readShadow(cBase_plug *pWriteTo, dConSig aCon){ DONT_USE_THIS; }
			virtual void writeShadow(const cBase_plug *pReadFrom, dConSig aCon){ DONT_USE_THIS; }
			virtual void appendShadow(const cBase_plug *pReadFrom, dConSig aCon){ DONT_USE_THIS; }
#		endif
	};

}

///////////////////////////////////////////////////////////////////////////////////
// Template implementation
namespace gt{

	//----------------------------------------------------------------------------------------------------------------
	template<typename A>
	typename tDataPlug<A>::dMapAssigns*
	getVoidAssignments(){
		static bool setup = false;
		static typename tDataPlug<A>::dMapAssigns ass;

		if(!setup){
			ass[ cBase_plug::genPlugType<A>() ] = voidAssign::basic<A>;
			setup=true;
		}

		return &ass;
	}

	template<typename A>
	typename tDataPlug<A>::dMapAppends*
	getVoidAppends(){
		static bool setup = false;
		static typename tDataPlug<A>::dMapAppends app;

		if(!setup){
			app[ cBase_plug::genPlugType<A>() ] = voidAppend::basic<A>;
			setup=true;
		}

		return &app;
	}

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

	template<typename T>
	cBase_plug&
	cBase_plug::operator= (const T &aFrom){
		//- there should be no need to chech for yourself due to input type

		if(cBase_plug::genPlugType<T>() != getType())
			excep::cantCopy("plug", "raw type", __FILE__, __LINE__);

		dynamic_cast< tDataPlug<T>* >(this)->get() = aFrom;	//- Attempt down cast.

		return *this;
	}

	//----------------------------------------------------------------------------------------------------------------
	template<typename A>
	cBase_plug::dPlugType
	tDataPlug<A>::getType() const {
		return cBase_plug::genPlugType<A>();
	}

	template<typename A>
	void
	tDataPlug<A>::assign(void *aTo, cBase_plug::dPlugType aType) const{
		PROFILE;

		tCoolFind<cBase_plug::dPlugType, fuAssign> assign(
			*getVoidAssignments<A>(),
			aType
		);

		if(!assign.found())
			throw excep::cantCopy(typeid(A).name(), "unknown type", __FILE__, __LINE__);

		assign.get()(
			&getConst(),
			aTo
		);
	}

	template<typename A>
	void
	tDataPlug<A>::append(void *aTo, cBase_plug::dPlugType aType) const{
		PROFILE;

		tCoolFind<cBase_plug::dPlugType, fuAssign> append(
			*getVoidAppends<A>(),
			aType
		);

		if(!append.found())
			throw excep::cantCopy(typeid(A).name(), "unknown type", __FILE__, __LINE__);

		append.get()(
			&getConst(),
			aTo
		);
	}

	template<typename A>
	bool
	tDataPlug<A>::operator== (const cBase_plug &pD) const {
		return (cBase_plug::genPlugType<A>() == pD.getType());
	}

}


#endif
