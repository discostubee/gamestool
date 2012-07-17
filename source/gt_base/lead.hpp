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

#ifndef	LEAD_HPP
#define LEAD_HPP

#include "command.hpp"
#include "context.hpp"
#include <boost/smart_ptr.hpp>

#define USE_TYPEINFO	//!< Enable or disable typeID as the method used to identify plug types.

#ifdef USE_TYPEINFO
	#include <typeinfo>
#endif

///////////////////////////////////////////////////////////////////////////////////
// Classes
namespace gt{
	template<typename T> class tPlug;

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
	enum eShadowMode{
		eSM_unset,
		eSM_read,
		eSM_write
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

	//--------------------------------------------------------------------------------------------------------
	//!\brief	There are 2 ways in which a figment can get data from another figments. Often it will be through
	//!			the context, but sometimes we want data from another context or to perform more complex tasks.
	//!			This is where the lead comes in: Figments are designed to have a generic interface, where calls
	//!			to manipulate it can be saved as part of a users program. In order to do this, an interface
	//!			call (known as jacking) uses a messenger class, in this case it's called a lead. A lead must
	//!			have a command so the figment getting jacked knows what to do with it. A lead then has multiple
	//!			plugs, some are labeled/tagged, while others are in an ordered pile.
	//!\note	Leads can not be contained by a plug.
	class cLead{
	public:
		const cCommand::dUID mCom;	//!< The command for this lead.

		cLead(cCommand::dUID aCom);
		cLead(const cLead &otherLead);
		~cLead();

		//!\brief	If it has the tagged plug, it returns a read only pointer to it. Throws if the plug isn't found.
		//!			Be careful not to store the pointer anywhere. Getting the plug this way is handier than passing
		//!			the plug in as an argument.
		//!\param	pTag	This is the tag we use to find out plug.
		const cBase_plug * getPlug(const cPlugTag* pTag);

		void addPlug(cBase_plug *addMe, const cPlugTag *aTag);	//!< Adds a tagged reference to a plug.

		//!\brief	if it has the tagged plug, it sets it to the value stored in the plug being passed in.
		void setPlug(cBase_plug *setMe, const cPlugTag *aTag, bool silentFail = false);

		//!\brief	Adds a piled reference to a plug.
		void addToPile(cBase_plug *addMe);

		//!\brief	Appends the vector with the contents of the pile.
		//!\note	Lists and queues are not supported because the pile is intended to be fast to index.
		template<typename PLUG_TYPE> void getPile(std::vector< tPlug<PLUG_TYPE> > *target);

		//!\brief	Lets you use direct types instead of plugs. Does this by copying to the input.
		//!\param	input	Pointer to where you want to copy the value.
		//!\param	tag		tag for the plug you wish to copy a value from.
		//!\param	silentFail	If set to true, this function won't throw if it can't find the plug or it can't copy it.
		//!				Use this if you don't care if the value was set or not.
		template<typename CONTAIN> void getValue( CONTAIN *input, const cPlugTag *tag, bool silentFail = false );

		//!\brief	Appends the vector with the values contained in the pile.
		template<typename VEC_TYPE> void getPileValues(std::vector< VEC_TYPE > *target);

		//!\brief	When a plug dies, it must let the lead know it is no longer valid.
		//!\brief	Needs to lock because this can come from anywhere.
		void unplug(cBase_plug* pPlug);

		#ifdef GT_THREADS
			class cLemming{
			public:
				cLead * mParent;
				cLemming(cLead *pLead) : mParent(pLead) { ++mParent->lemmingCount; }
				cLemming(const cLemming& other) : mParent(other.mParent) { ++mParent->lemmingCount; }
				~cLemming() { if(mParent) mParent->lemmingCallback(); }
			};

			cLemming startLead(cContext* pCon);	//!< Apart from the unplug function, this is only thread-locked function as it's the only way to ensure there is no thread collisions.
		#endif

	protected:
		typedef std::map<cPlugTag::dUID, cBase_plug*> dDataMap;
		typedef std::list<cBase_plug*> dPiledData;
		
		#ifdef GT_THREADS
			typedef boost::lock_guard<boost::recursive_mutex> dUseLock;

			boost::recursive_mutex mMutex;
			cContext* mCurrentCon;
			short lemmingCount;

			void lemmingCallback();

			friend class cLemming;
		#endif

		dDataMap mTaggedData; 	//!<
		dPiledData mDataPile;	//!<

		dDataMap::iterator scrTDataItr;
		dPiledData::iterator scrPDataItr;

	private:
		//!\brief	bad
		cLead& operator = (const cLead &aOtherLead);
	};
}

///////////////////////////////////////////////////////////////////////////////////
// Templates
namespace gt{

	//--------------------------------------------------------------------------------------------------------
	template<typename PLUG_TYPE>
	void
	cLead::getPile(std::vector< tPlug<PLUG_TYPE> > *target){
		target->reserve(target->size() + mDataPile.size());
		for(scrPDataItr = mDataPile.begin(); scrPDataItr != mDataPile.end(); ++scrPDataItr){
			#ifdef GT_THREADS
				ASRT_NOTNULL(mCurrentCon);
				target->push_back( (*scrPDataItr)->getShadow(mCurrentCon->getSig(), eSM_read) );
			#else
				 target->push_back( (*scrPDataItr) );
			#endif
		}
	}

	template<typename CONTAIN>
	void
	cLead::getValue( CONTAIN *input, const cPlugTag *tag, bool silentFail ){
		scrTDataItr = mTaggedData.find(tag->mID);
		if(scrTDataItr != mTaggedData.end()){
			#ifdef GT_THREADS
				ASRT_NOTNULL(mCurrentCon);
				scrTDataItr->second->getShadow(mCurrentCon->getSig(), eSM_read)->copyInto(input);
			#else
				scrTDataItr->second->copyInto(input);
			#endif
		}else if(!silentFail){
			std::stringstream ss; ss << "plug " << &tag->mName;
			throw excep::notFound(ss.str().c_str(), __FILE__, __LINE__);
		}
	}

	template<typename VEC_TYPE>
	void
	cLead::getPileValues(std::vector< VEC_TYPE > *target){
		target->reserve(target->size() + mDataPile.size());
		for(scrPDataItr = mDataPile.begin(); scrPDataItr != mDataPile.end(); ++scrPDataItr){
			target->push_back(VEC_TYPE());
			#ifdef GT_THREADS
				ASRT_NOTNULL(mCurrentCon);
				(*scrPDataItr)->getShadow(mCurrentCon->getSig(), eSM_read)->copyInto( &target->back() );
			#else
				 (*scrPDataItr)->copyInto( &target->back() );
			#endif
		}
	}

	//--------------------------------------------------------------------------------------------------------
	template<typename PLUG_TYPE>
	cBase_plug::dPlugType
	cBase_plug::getPlugType(){
		static dPlugType typeID = 0;
		if(typeID == 0){
			typeID = makeHash(typeid(PLUG_TYPE).name());
		}
		return typeID;
	}

	template< typename T>
	void
	cBase_plug::copyInto(T *container) const{
		actualCopyInto(reinterpret_cast<void*>(container), getPlugType<T>());
	}

}

/*
//!\brief
struct sSortDMap{
	bool operator () (const dDataMap::iterator &pA, const dDataMap::iterator &pB) const {
		if(pA->first > pB->first)
			return true;
		else
			return false;
	}
};

//!\brief
struct sSortPile{
	bool operator () (const dPiledData::iterator &pA, const dPiledData::iterator &pB) const {
		if(*pA > *pB) // just compare memory addresses
			return true;
		else
			return false;
	}
};
*/


#endif
