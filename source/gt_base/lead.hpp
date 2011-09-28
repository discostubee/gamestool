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
	#define PLUG_CANT_COPY(t) throw excep::cantCopy("", typeid(t).name(), __FILE__, __LINE__)
	#define PLUG_CANT_COPY_ID(t) throw excep::cantCopy("", t.name(), __FILE__, __LINE__)
#else
	typedef const dNameHash PLUG_TYPE_ID;
	#define PLUG_TYPE_TO_ID(t) nameHash(typeid(t).name())
	#define PLUG_CANT_COPY(t) throw excep::cantCopy("", "", __FILE__, __LINE__)
	#define PLUG_CANT_COPY_ID(t) throw excep::cantCopy("", "", __FILE__, __LINE__)
#endif

///////////////////////////////////////////////////////////////////////////////////
// Classes
namespace gt{

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
	class cUpdateLemming{
	private:
		cBase_plug *callMe;

	public:
		explicit cUpdateLemming(cBase_plug *callBack);
		~cUpdateLemming();
	};

	//----------------------------------------------------------------------------------------------------------------
	//!\brief	The interface for the plug template below.
	class cBase_plug{
	public:
		PLUG_TYPE_ID mType;	//!< Must be public so the tPlug templates can use it.

		cBase_plug(PLUG_TYPE_ID pTI);
		cBase_plug(const cBase_plug& pCopy);

		//template<typename T>	cBase_plug& operator= (const T &pT);

		template< template<typename> class plug, typename T>	cBase_plug& operator= (const plug<T> &pT);

		template<typename T> T getCopy();

		template<typename T> T* getPtr();

		//--- Intended to be polymorphed by descendants.
		virtual ~cBase_plug();

		virtual	cBase_plug&	operator= (const cBase_plug &pD) //=0;	//- should be pure virtual
			{	DUMB_REF_ARG(pD); DONT_USE_THIS; return *this; }

		virtual	cBase_plug& operator= (const cBase_plug *pD) //=0;	//- should be pure virtual
			{	DUMB_REF_ARG(pD); DONT_USE_THIS; return *this; }

		virtual cBase_plug* getShadow(dConSig aCon, eShadowMode whatFor) =0; //!< Leads must always work with shadows.
		virtual cUpdateLemming update() =0; //!< locks all the connected leads and updates the shadows. The update finished when the lemming dies.
		virtual void linkLead(cLead* pLead) =0; //!< Leads must let the plug know that they are linked in.
		virtual void unlinkLead(cLead* pLead) =0; //!< When a plug is destroyed, it must let the lead know.

	protected:
		virtual void finishUpdate() =0; //!< used only by the update lemming.

	friend class cUpdateLemming;
	};

	//--------------------------------------------------------------------------------------------------------
	//!\brief	Figments are designed to have a generic interface, where calls to manipulate it can be saved as part of
	//!			a users program. In order to do this, an interface call (known as jacking) uses a messenger
	//!			class, in this case it's called a lead. A lead must have a command so the figment getting
	//!			jacked knows what to do with it. A lead then has multiple plugs, some are labeled/tagged, while
	//!			others are in an ordered pile.
	//!\note	This 1 lead per context means that the number of shadows in a plug are conserved and the owner
	//!			of a user of leads must explicitly setup more if they want to support a new context.
	//!\todo	Speed this up by only allowing plugs which are used by the command.
	class cLead{
	public:

		const cCommand::dUID mCom;	//!< The command for this lead.
		const dConSig mConx;		//!< You can only have 1 lead per context which must match when jacking.

		//!\brief
		//!\param	aCom	Link to the command we want this plug to use.
		//!\param	aConx
		cLead( cCommand::dUID aCom, dConSig aConx);

		//!\brief	Copies other lead.
		cLead(const cLead &otherLead);

		~cLead();

		//!\brief	If it has the tagged plug, it makes the input data equal that of its plug. Otherwise the output is untouched.
		//!\note	Doesn't lock because it should only be used within a jack which locks leads for you.
		void getPlug(cBase_plug *aOutPlug, const cPlugTag* pTag);

		//!\brief	Add a single tagged plug.
		//!\note	Doesn't lock because it should only be used within a jack which locks leads for you.
		void addPlug(cBase_plug *aPlug, const cPlugTag *aTag);

		//!\brief	sets the value of this leads plug to same as the incoming plug.
		void setPlug(cBase_plug *aPlug, const cPlugTag *aTag);

		//!\brief	Appends another plug to the end of the pile.
		void addToPile(cBase_plug *aPlug);

		//!\brief	Clears target and copies the leads pile into it. Expects tPlug type
		template<typename C> void getPile(std::vector< C > target){
			target.clear();
			target.reserve(mDataPile.size());
			for(scrPDataItr = mDataPile.begin(); scrPDataItr != mDataPile.end(); ++scrPDataItr){
				target.push_back( *((*scrPDataItr)->getShadow(mConx, eSM_read)) );
			}
		}

		//--- Things for plugs.
		#ifdef GT_THREADS
			typedef boost::unique_lock<boost::mutex> dLockLead;

			boost::mutex muLead;	//!< Used by plugs and figment jacks to lock the lead.
		#endif

		//!\brief	When a plug dies, it must let the lead know it is no longer valid.
		//!\note	Locks until finished because this can come from any thread at any time.
		void unplug(cBase_plug* pPlug);

	private:
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

#endif
