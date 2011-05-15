/*
 * !\file	lead.hpp
 * !\brief
 *
 * cSomething	A class name.
 * sSomething	A structure name.
 * eSomething	an enum, for both the scope and the values.
 * mSomething	Variable data stored in a class or structure, otherwise known as a member variable.
 * pSomething	A parameter passed into a function call.
 * nSomething	A name space.
 * uSomething	Constant and constant static data where the u stands for unchanging. Not to replace p when constant data is passed to a function.
 * tSomething	A template class or struct.
 * xSomething	Static data as either a class member or a function variable.
 * dSomething	A type definition.
 * SOMETHING	A pre-processor macro or value.
 * gSomething	A global variable.
 *
 */

#ifndef	LEAD_HPP
#define LEAD_HPP

#include "plug.hpp"
#include <boost/smart_ptr.hpp>

///////////////////////////////////////////////////////////////////////////////////
// Classes
namespace gt{

	//--------------------------------------------------------------------------------------------------------
	// A lead allows you to connect data from any number of different
	// figments and then jack them into another figment. Leads use
	// references to a command in order to identify what they are
	// meant to do. Once a lead has one of its plugs connected to the
	// data in a figment, that plug continues to reference the live
	// data.
	class cLead{
	public:
		typedef std::map<unsigned int, cBase_plug*> dDataMap;
		typedef std::list<cBase_plug*> dPiledData;

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

			cBase_plug* getPlug(){
				return *mItr;
			}

			void operator ++ (){
				++mItr;
			}
		};

		const cCommand*	mCom;

		cLead(const cCommand* pCom);
		~cLead();

		//!\fn		void add(cBase_plug* pData, const cPlugTag &pTag)
		//!\brief	Add a reference to a plug and assign it a label
		void add(cBase_plug* pData, const cPlugTag* pTag);

		//!\fn		void addToPile(cBase_plug* pData)
		//!\brief	Chunk a plug into the pile of other plugs.
		void addToPile(cBase_plug* pData);

		//!\fn		void take(cBase_plug** pData, const cPlugTag &pTag)
		//!\brief	Use this function if you want the lead to manage the plug instead of using a reference.
		//!\param	pData
		//!\param	pTag	The tag to give this data.
		void take(cBase_plug* pData, const cPlugTag* pTag);

		void takeToPile(cBase_plug* pData);

		cBase_plug* getD(const cPlugTag* pTag);	//!< Get data by its tag.

		cPileItr getPiledDItr();

		//!\fn		void unplug(cBase_plug* pPlug)
		//!\brief	When a plug dies, it must let the lead know it is no longer valid.
		//!\note	Does not tell the plug that this lead has been unplugged, because we should be
		//!		be able to assume the plug already knows.
		void unplug(cBase_plug* pPlug);

		void clear();

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

#endif
