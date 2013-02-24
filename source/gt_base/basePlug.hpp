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
		template<typename PLUG_TYPE> static dPlugType getPlugType();

		//--- implemented
		const dPlugType mType;	//!< Must be public so the tPlug templates can use it.

		cBase_plug(dPlugType pTI);
		cBase_plug(const cBase_plug& pCopy);	//!< Only copies the type. Assume later implementations will copy contents.
		virtual ~cBase_plug();

		virtual void linkLead(cLead* pLead); //!< Add a new link, or increase the number of times this lead is linked to this plug.	!\note	Made threadsafe in implementation.
		virtual void unlinkLead(cLead* pLead); //!< Decrements the number of links, only disconnecting when there is 0 links to this lead. !\note	Made threadsafe in implementation.

		template<typename T> void copyInto(T *container) const;	//!< The plug will try and copy itself into the given memory location.
		template<typename T> void copyFrom(const T *container);	//!< The plug will try and copy the value from the container.

		//--- interface
		//!\brief Appends the buffer with binary data that should be understandable by any platform.
		virtual void save(cByteBuffer* pSaveHere) = 0;

		//!\brief Reloads data from the buffer and delets the contents it used (because save or loading is a one to one operation).
		virtual void loadEat(cByteBuffer* pChewToy, dReloadMap *aReloads = NULL) = 0;

		virtual	cBase_plug& operator= (const cBase_plug &pD) =0;	//!< Assigns only the content, does not copy any linked lead info.
		virtual bool operator== (const cBase_plug &pD) const =0;

		#ifdef GT_THREADS
			virtual void updateStart() =0;	//!< Write shadow updates to origin.
			virtual void updateFinish() =0;	//!< Update all shadows using the origin. Any shadow writes after the update was started are overwritten.
		#endif

		//--- should be protected.
		typedef std::map<cLead*, unsigned int> dMapLeads;

		dMapLeads mLeadsConnected;		//!< Lead connections are not copied when copy plug values.
		dMapLeads::iterator itrLead;	//!< handy.

		virtual void actualCopyInto(void* pContainer, dPlugType pType) const =0;
		virtual void actualCopyFrom(const void* pContainer, dPlugType pType) =0;

		#ifdef GT_THREADS
			virtual void readShadow(cBase_plug *pWriteTo, dConSig aCon) =0;
			virtual void writeShadow(const cBase_plug *pReadFrom, dConSig aCon) =0;
		#endif

	friend class cLead;
	};

}


namespace gt{

	template<typename PLUG_TYPE>
	cBase_plug::dPlugType
	cBase_plug::getPlugType(){
		static dPlugType typeID = 0;

		if(typeID == 0)
			typeID = makeHash(typeid(PLUG_TYPE).name());

		return typeID;
	}

	template<typename T>
	void
	cBase_plug::copyInto(T *container) const{
		actualCopyInto(static_cast<void*>(container), getPlugType<T>());
	}

	template<typename T>
	void
	cBase_plug::copyFrom(const T *container){
		actualCopyFrom(static_cast<const void*>(container), getPlugType<T>());
	}
}

#endif
