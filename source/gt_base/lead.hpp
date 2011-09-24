/*
 * !\file	lead.hpp
 * !\brief
 *
 */

#ifndef	LEAD_HPP
#define LEAD_HPP

#include "plug.hpp"
#include "context.hpp"
#include <boost/smart_ptr.hpp>

///////////////////////////////////////////////////////////////////////////////////
// Classes
namespace gt{

	//--------------------------------------------------------------------------------------------------------
	//!\brief	Figments are designed to have a generic interface, where calls to manipulate it can be saved as part of
	//!			a users program. In order to do this, an interface call (known as jacking) uses a messenger
	//!			class, in this case it's called a lead. A lead must have a command so the figment getting
	//!			jacked knows what to do with it. A lead then has multiple plugs, some are labeled/tagged, while
	//!			others are in an ordered pile. In order to avoid deadlocks and handle multithreading, the lead
	//!			is context aware. That way, only plugs from the same context can be added.
	class cLead{
	public:
		typedef std::map<unsigned int, cBase_plug*> dDataMap;
		typedef std::list<cBase_plug*> dPiledData;

		//!\brief	Used for piled data.
		class cPileItr{
		private:
			const dPiledData* mPile;
			dPiledData::iterator mItr;

		public:
			cPileItr(dPiledData* pPile):
				mPile(pPile), mItr(pPile->begin())
			{}

			~cPileItr(){
			}

			bool atEnd(){
				if(mItr == mPile->end())
					return true;

				return false;
			}

			//!\todo Change to a reference return.
			cBase_plug* getPlug(){
				return *mItr;
			}

			void operator ++ (){
				++mItr;
			}
		};

		const cCommand::dUID mCom;	//!< The command for this lead.
		dConSig mConx;				//!< A lead can only let you play with it if you're from the same context.

		//!\param	aCom	Link to the command we want this plug to use.
		//!\param	aCon	To avoid deadlocks and having to mutex lock stuff all the time,
		//!					a lead can only add plugs from the same context.
		cLead(cCommand::dUID aCom, dConSig aConx);

		~cLead();

		//!\brief	Add a reference to a plug and assign it a label
		//!\param	aPlug	The plug we want.
		//!\param	aTag	This is what the plug is known as in this lead.
		//!\param	aCon	To avoid deadlocks and having to mutex lock stuff all the time,
		//!					a lead can only add plugs from the same context.
		void add(cBase_plug *aPlug, const cPlugTag *aTag, dConSig aCon);

		//!\brief	Allows you to add a plug using the tag's unique ID.
		void add(cBase_plug *aPlug, cPlugTag::dUID ID, dConSig aCon);

		//!\brief	Chuck a plug into the pile of other plugs.
		//!\param	aPlug	The plug we want.
		//!\param	aCon	Same as the add foo above, we need to know the context.
		void addToPile(cBase_plug *aData, dConSig aCon);

		//!\brief	Lets you play with another figments plug, but only if you're from the same
		//!			context, and only if you play nice.
		cBase_plug* getPlug(const cPlugTag *aTag, dConSig aCon);

		//!\brief	Lets you go through all the piled data.
		cPileItr getPiledDItr(dConSig aCon);

		//!\brief	An easy way to set the plug
		void setPlug(cBase_plug *aPlug, const cPlugTag *aTag, dConSig aCon);

		//!\brief	Clears only the linked plugs. The command link and context remains the same
		void clear();

	protected:

		//!\brief	When a plug dies, it must let the lead know it is no longer valid.
		//!\note	Does not tell the plug that this lead has been unplugged, because we should be
		//!		be able to assume the plug already knows.
		void unplug(cBase_plug* pPlug);

	friend class cBase_plug;

	private:

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

		typedef std::set<dDataMap::iterator, sSortDMap> dTagCleanup;
		typedef std::set<dPiledData::iterator, sSortPile> dPileCleanup;
		
		dDataMap mTaggedData; //!< Data which is identified by
		dPiledData mDataPile;
		dTagCleanup mTaggedCleanup;	//!<iterators to things in mTaggedData to clean up.
		dPileCleanup mPileCleanup; //!< iterators to things in the piled data to clean up.

		dDataMap::iterator scrTDataItr;
		dPiledData::iterator scrPDataItr;
		dTagCleanup::iterator scrTCleanItr;
		dPileCleanup::iterator scrPCleanItr;
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
