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
// Forward decs
namespace gt{
	class cLead;
	template<typename T> class tPlug;
}

///////////////////////////////////////////////////////////////////////////////////
// Other types
namespace gt{
	#ifdef GT_THREADS
		//!\brief	Used to indicate how shadows are effects, and how the effect the source.
		enum eShadowMode{
			eSM_unset,
			eSM_read,	//!< Data read
			eSM_link,	//!< The links have changed and the source needs to change accordingly.
			eSM_write,	//!< Data is written to the source.
			eSM_all		//!< Update both the data and the link
		};
	#endif
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
		template<typename PLUG_TYPE> static dPlugType getPlugType();

		//--- implemented
		const dPlugType mType;	//!< Must be public so the tPlug templates can use it.

		cBase_plug(dPlugType pTI);
		cBase_plug(const cBase_plug& pCopy);

		virtual void linkLead(cLead* pLead); //!< Add a new link, or increase the number of times this lead is linked to this plug.	!\note	Made threadsafe in implementation.
		virtual void unlinkLead(cLead* pLead); //!< Decrements the number of links, only disconnecting when there is 0 links to this lead. !\note	Made threadsafe in implementation.

		//--- interface
		virtual ~cBase_plug();

		//!\brief Appends the buffer with binary data that should be understandable by any platform.
		virtual void save(cByteBuffer* pSaveHere) = 0;

		//!\brief Reloads data from the buffer and delets the contents it used (because save or loading is a one to one operation).
		virtual void loadEat(cByteBuffer* pChewToy, dReloadMap *aReloads = NULL) = 0;

		virtual	cBase_plug& operator= (const cBase_plug &pD) =0;
		virtual bool operator== (const cBase_plug &pD) const =0;

		template<typename T> void copyInto(T *container) const;	//!< The plug will try and copy itself into the given memory location.

		#ifdef GT_THREADS
			virtual void updateStart() =0;
			virtual void updateFinish() =0;
		#endif

		size_t numLeadsConnected();

	protected:
		typedef std::map<cLead*, unsigned int> dMapLeads;

		dMapLeads mLeadsConnected;		//!< Lead connections are not copied when copy plug values.
		dMapLeads::iterator itrLead;	//!< handy.

		virtual void actualCopyInto(void* pContainer, dPlugType pType) const =0;

		#ifdef GT_THREADS
			virtual cBase_plug* getShadow(dConSig aCon, eShadowMode whatFor) =0; //!< Leads must always work with shadows.
		#endif

	friend class cLead;
	};

}


#endif
