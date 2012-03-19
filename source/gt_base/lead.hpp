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
// Macros

#ifdef USE_TYPEINFO
	typedef const std::type_info & PLUG_TYPE_ID;
	#define PLUG_TYPE_TO_ID(t) typeid(t)
	#define PLUG_CANT_COPY(copier, copiee) throw excep::cantCopy(typeid(copier).name(), typeid(copiee).name(), __FILE__, __LINE__)
	#define PLUG_CANT_COPY_ID(copier, copiee) throw excep::cantCopy(copier.name(), copiee.name(), __FILE__, __LINE__)
#else
	typedef const dNameHash PLUG_TYPE_ID;
	#define PLUG_TYPE_TO_ID(t) nameHash(typeid(t).name())
	#define PLUG_CANT_COPY(copier, copiee) throw excep::cantCopy("", "", __FILE__, __LINE__)
	#define PLUG_CANT_COPY_ID(copier, copiee) throw excep::cantCopy("", "", __FILE__, __LINE__)
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
			mName( PCStrToNStr(pPlugName) ),
			mID( makeHash(mName.c_str()) )
		{}

		~cPlugTag()
		{}

	private:
		cPlugTag& operator = (const cPlugTag&){ return *this; }
	};

	//----------------------------------------------------------------------------------------------------------------
	enum eShadowMode{
		eSM_read,
		eSM_write
	};

	#ifdef GT_THREADS
		//----------------------------------------------------------------------------------------------------------------
		//!\brief	Used when updating a plug.
		class cUpdateLemming{
		private:
			cBase_plug *callMe;

		public:
			explicit cUpdateLemming(cBase_plug *callBack);
			~cUpdateLemming();
		};

		#define PLUGUP(plug) cUpdateLemming lem__LINE__ = plug.update()

	#else
		#define PLUGUP(plug) (void)plug
	#endif

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
		const PLUG_TYPE_ID mType;	//!< Must be public so the tPlug templates can use it.

		cBase_plug(PLUG_TYPE_ID pTI);
		cBase_plug(const cBase_plug& pCopy);

		template< template<typename> class PLUG, typename T> cBase_plug& operator= (const PLUG<T> &pT);
		template<typename T> void copyInto(T *container, bool silentFail = false) const;

		virtual void linkLead(cLead* pLead); //!< Add a new link, or increase the number of times this lead is linked to this plug.	!\note	Must be threadsafe.
		virtual void unlinkLead(cLead* pLead); //!< Decrements the number of links, only disconnecting when there is 0 links to this lead. !\note	Must be threadsafe.

		//--- Intended to be polymorphed by descendants.
		virtual ~cBase_plug();

		virtual	cBase_plug& operator= (const cBase_plug &pD) =0;
		virtual bool operator== (const cBase_plug &pD) =0;

	#ifdef GT_THREADS
		virtual cUpdateLemming update() =0; //!< locks all the connected leads and updates the shadows. The update finished when the lemming dies.
	#endif

	protected:
		typedef std::map<cLead*, unsigned int> dMapLeads;

		dMapLeads mLeadsConnected;		//!< Lead connections are not copied when copy plug values.
		dMapLeads::iterator itrLead;	//!< handy.

		#ifdef GT_THREADS
			virtual cBase_plug* getShadow(dConSig aCon, eShadowMode whatFor) =0; //!< Leads must always work with shadows.

			virtual void finishUpdate() =0; //!< used only by the update lemming.

			friend class cUpdateLemming;
		#endif

	friend class cLead;
	};

	//--------------------------------------------------------------------------------------------------------
	//!\brief	Figments are designed to have a generic interface, where calls to manipulate it can be saved as part of
	//!			a users program. In order to do this, an interface call (known as jacking) uses a messenger
	//!			class, in this case it's called a lead. A lead must have a command so the figment getting
	//!			jacked knows what to do with it. A lead then has multiple plugs, some are labeled/tagged, while
	//!			others are in an ordered pile.
	//!\note	The ability to get and set plugs do not lock because they should only be used in the jack function
	//!			which performs a single lock on the lead, which prevents the connected plugs from messing with it
	//!			when they update or unlink.
	//!\note	Leads can not be contained by a plug.
	class cLead{
	public:
		const cCommand::dUID mCom;	//!< The command for this lead.
		dConSig mConx;			//!< You can only have 1 lead per context which must match when jacking.

		//!\brief
		//!\param	aCom	Link to the command we want this plug to use.
		//!\param	aConx
		cLead(cCommand::dUID aCom, dConSig aConx);

		//!\brief	Copies other lead.
		cLead(const cLead &otherLead);

		~cLead();

		//--- These things should only be used by the jack. Currently not protected so that unit tests can fudge use them.

		//!\brief	If it has the tagged plug, it returns a pointer to it. Throws if the plug isn't found. Be careful not to store
		//!			the pointer anywhere. Getting the plug this way is handier than passing the plug in as an argument.
		//!\param	pTag	This is the tag we use to find out plug.
		//!\note	Assumes you only want to read from the plug.
		cBase_plug * getPlug(const cPlugTag* pTag);

		void addPlug(cBase_plug *addMe, const cPlugTag *aTag);

		void setPlug(const cBase_plug *setMe, const cPlugTag *aTag, bool silentFail = false);	//!< Takes the value of a plug in this lead,and assigns that value to the plug passed in.

		void addToPile(cBase_plug *addMe);

		//!\brief	Clears target and copies the leads pile into it. Expects tPlug type
		template<typename C> void getPile(std::vector< C > *target){
			target->clear();
			target->reserve(mDataPile.size());
			for(scrPDataItr = mDataPile.begin(); scrPDataItr != mDataPile.end(); ++scrPDataItr){
				target->push_back( C() );
				#ifdef GT_THREADS
					target->back() = (*scrPDataItr)->getShadow(mConx, eSM_read);
				#else
					target->back() = *scrPDataItr;
				#endif
			}
		}

		//!\brief	Lets you use direct types instead of plugs. Does this by copying to the input.
		//!\param	input	Pointer to where you want to copy the value.
		//!\param	tag		tag for the plug you wish to copy a value from.
		//!\param	silentFail	If set to true, this function won't throw if it can't find the plug or it can't copy it.
		//!				Use this if you don't care if the value was set or not.
		template<typename C> void getValue( C *input, const cPlugTag *tag, bool silentFail = false ){
			scrTDataItr = mTaggedData.find(tag->mID);
			if(scrTDataItr != mTaggedData.end()){
				#ifdef GT_THREADS
					scrTDataItr->second->getShadow(mConx, eSM_read)->copyInto(input);
				#else
					scrTDataItr->second->copyInto(input);
				#endif
			}else if(!silentFail){
				std::stringstream ss; ss << "plug " << tag->mName;
				throw excep::notFound(ss.str().c_str(), __FILE__, __LINE__);
			}
		}

		//--- Things for plugs. not protected because there are a lot of templates that need them.
		#ifdef GT_THREADS
			typedef boost::lock_guard<boost::recursive_mutex> dLockLead;

			boost::recursive_mutex muLead;	//!< Used by plugs and figment jacks to lock the lead.
		#endif

		//!\brief	When a plug dies, it must let the lead know it is no longer valid.
		//!\note	Locks until finished because this can come from any thread at any time.
		void unplug(cBase_plug* pPlug);

	protected:
		typedef std::map<cPlugTag::dUID, cBase_plug*> dDataMap;
		typedef std::list<cBase_plug*> dPiledData;

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
		
		dDataMap mTaggedData; 	//!<
		dPiledData mDataPile;	//!<

		dDataMap::iterator scrTDataItr;
		dPiledData::iterator scrPDataItr;

	private:
		//!\brief	bad
		cLead& operator = (const cLead &aOtherLead);
	};
}

namespace excep{
	class badContext : public base_error{
	public:
		badContext(const char* pFile, const unsigned int pLine) :
			base_error(pFile, pLine)
		{ addInfo("bad context"); }

		virtual ~badContext() throw() {}
	};
}

///////////////////////////////////////////////////////////////////////////////////
// Templates
namespace gt{

	template<typename T>
	void
	cBase_plug::copyInto(T *container, bool silentFail) const{
		if(mType != PLUG_TYPE_TO_ID(T)){
			if(!silentFail)
				PLUG_CANT_COPY_ID(mType, typeid(T));
			else
				return;
		}

		*container = dynamic_cast< const tPlug<T>* >(this)->mD;
	}

	template< template<typename> class PLUG, typename T>
	cBase_plug&
	cBase_plug::operator= (const PLUG<T> &pT){
		if(this != &pT){
			if(mType != pT.mType)
				PLUG_CANT_COPY_ID(mType, pT.mType);

			dynamic_cast< tPlug<T>* >(this)->mD = pT.mD;
		}else{
			WARN("type mismatch when assigning");
		}
		return *this;
	}

}


#endif
