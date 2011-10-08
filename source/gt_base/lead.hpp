/*
 * !\file	lead.hpp
 * !\brief
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

		const dUID	mID;
		const dStr	mName;

		cPlugTag(
			const char* pPlugName
		):
			mID( makeHash(pPlugName) ),
			mName( pPlugName )
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

	//----------------------------------------------------------------------------------------------------------------
	//!\brief	Used when updating a plug.
	class cUpdateLemming{
	private:
		cBase_plug *callMe;

	public:
		explicit cUpdateLemming(cBase_plug *callBack);
		~cUpdateLemming();
	};

	//----------------------------------------------------------------------------------------------------------------
	//!\brief	A plug is a data container that a lead can connect too. The lead can then connect that data to another
	//!			object via the jack function, as well as automatically disconnect itself from its linked leads when it
	//!			dies. It is also designed for serialization using a byte buffer.
	//!\note	You don't have to use plugs for all your figments stuff, Just for the things you want to save and
	//!			reload or pass through a lead to another object.
	//!\note	Using assignment operators doesn't use the other plugs shadows. This is because shadows are only for
	//!			leads, and shadows are meant to solve the issue of data access over different threads. Inter-thread
	//!			access should only happen through leads.
	//!\note	Plugs do not save their connections, this is the job of the reflection object.
	class cBase_plug{
	public:
		const PLUG_TYPE_ID mType;	//!< Must be public so the tPlug templates can use it.

		cBase_plug(PLUG_TYPE_ID pTI);
		cBase_plug(const cBase_plug& pCopy);

		template< template<typename> class plug, typename T>	cBase_plug& operator= (const plug<T> &pT);

		template<typename T> T* exposePtr();	//!< Be careful with this.

		template<typename T> void copyInto(T *container, bool silentFail = false) const;

		//--- Intended to be polymorphed by descendants.
		virtual ~cBase_plug();

		virtual	cBase_plug&	operator= (const cBase_plug &pD) //=0;	//- should be pure virtual
			{	DUMB_REF_ARG(pD); DONT_USE_THIS; return *this; }

		virtual	cBase_plug& operator= (const cBase_plug *pD) //=0;	//- should be pure virtual
			{	DUMB_REF_ARG(pD); DONT_USE_THIS; return *this; }

		#ifdef GT_THREADS
			virtual cBase_plug* getShadow(dConSig aCon, eShadowMode whatFor) =0; //!< Leads must always work with shadows.
			virtual cUpdateLemming update() =0; //!< locks all the connected leads and updates the shadows. The update finished when the lemming dies.
		#endif

		virtual void linkLead(cLead* pLead) =0; //!< Add a new link, or increase the number of times this lead is linked to this plug.	!\note	Must be threadsafe.
		virtual void unlinkLead(cLead* pLead) =0; //!< Decrements the number of links, only disconnecting when there is 0 links to this lead. !\note	Must be threadsafe.

	protected:
		#ifdef GT_THREADS
			virtual void finishUpdate() =0; //!< used only by the update lemming.
		#endif

	friend class cUpdateLemming;
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

		void setPlug(cBase_plug *setMe, const cPlugTag *aTag, bool silentFail = false);	//!< Assigns the value in this lead to the input input plug

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
	T*
	cBase_plug::exposePtr(){
		if(mType != PLUG_TYPE_TO_ID(T))
			PLUG_CANT_COPY_ID(mType, typeid(T));

		return &dynamic_cast< tPlug<T>* >(this)->mD;
	}

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

	template< template<typename> class plug, typename T>
	cBase_plug&
	cBase_plug::operator= (const plug<T> &pT){
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
