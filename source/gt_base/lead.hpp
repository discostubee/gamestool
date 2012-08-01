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

///////////////////////////////////////////////////////////////////////////////////
// Includes

#include "basePlug.hpp"

#include <boost/smart_ptr.hpp>

///////////////////////////////////////////////////////////////////////////////////
// Classes
namespace gt{

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

		//!\brief	Adds a tagged reference to a plug to our lead.
		//!\param	addMe	Plug to connect to lead.
		//!\param	pTag	This is the tag we use to find our plug.
		void addPlug(cBase_plug *addMe, const cPlugTag *pTag);

		//!\brief	Get a plug from a lead by copying its value into another plug.
		//!\param	setMe	The plug we want to copy into.
		//!\param	pTag	This is the tag we use to find our plug.
		//!\return	False if plug not found
		bool getPlug(cBase_plug *setMe, const cPlugTag* pTag);

		//!\brief	If it has the tagged plug, it sets it to the value stored in the plug being passed in.
		//!\param	pTag	This is the tag we use to find our plug.
		bool setPlug(const cBase_plug *copyMe, const cPlugTag *pTag);

		//!\brief	If you want to pass a plug from one lead to another, here's how you do it.
		//!\param	passTo	This is the lead that will get the plug from this lead.
		//!\param	pGetTag	This is the tag used to find the plug in this lead.
		//!\param	pPutTag	(optional) If specified, the target lead will be assigned the plug using this
		//!				put tag instead
		//!\return	false if the plug wasn't found using the get tag.
		bool passPlug(cLead *passTo, const cPlugTag *pGetTag, const cPlugTag *pPutTag = NULL);

		//!\brief	Lets you use direct types instead of plugs. Does this by copying to the input.
		//!\param	input	Pointer to where you want to copy the value.
		//!\param	tag		tag for the plug you wish to copy a value from.
		//!\return	false if it couldn't find the plug
		template<typename CONTAIN> bool getValue( CONTAIN *input, const cPlugTag *tag);

		//!\brief	If you don't want to re-create a lead without this plug, or you don't want to over-ride it, you
		//!			can remove it with this.
		void remPlug(const cPlugTag *pGetTag);

		//!\brief	Adds a piled reference to a plug.
		void addToPile(cBase_plug *addMe);

		//!\brief	If you need to add a tagged plug to another lead's pile, do this:
		void passToPile(cLead *passTo, const cPlugTag *pGetTag);

		//!\brief	Adds one lead's pile to the end of this lead.
		void addPile(cLead *passTo);

		//!\brief
		template<typename PLUG_TYPE> void getPile(std::vector< tPlug<PLUG_TYPE> > *target);

		//!\brief	Clears out the pile. Provided for those times when instead of making a new lead, you want to keep
		//!			the tagged data.
		void clearPile();

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

			cLemming startLead(cContext* pCon);
			dConSig getCurrentSig() const;
		#endif

	protected:
		typedef std::map<cPlugTag::dUID, cBase_plug*> dDataMap;
		typedef std::list<cBase_plug*> dPiledData;
		
		#ifdef GT_THREADS
			typedef boost::lock_guard<boost::recursive_mutex> dLock;

			boost::recursive_mutex mu;
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

	template<typename CONTAIN>
	bool
	cLead::getValue( CONTAIN *input, const cPlugTag *tag){
		scrTDataItr = mTaggedData.find(tag->mID);

		if(scrTDataItr == mTaggedData.end())
			return false;

		scrTDataItr->second->copyInto(input);
		return true;
	}

  	template<typename PLUG_TYPE>
	void
	cLead::getPile(std::vector< tPlug<PLUG_TYPE> > *target){
		target->reserve(target->size() + mDataPile.size());
		for(scrPDataItr = mDataPile.begin(); scrPDataItr != mDataPile.end(); ++scrPDataItr){
#			ifdef GT_THREADS
				ASRT_NOTNULL(mCurrentCon);
				target->push_back( tPlug<PLUG_TYPE>() );
				(*scrPDataItr)->readShadow( &target->back(), mCurrentCon->getSig() );
#			else
				 target->push_back( (*scrPDataItr) );
#			endif
		}
	}

}
/*
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

///////////////////////////////////////////////////////////////////////////////////
// Macros
#ifdef GTUT
#	ifdef GT_THREADS
		//!\brief	Used so you can get and set plugs inside a unit test, where normally this can only be done within a jack function.
#		define FAUX_JACK(lead, context) cLead::cLemming lemLock = lead->startLead(&context);
#	else
#		define FAUX_JACK(lead, context)
#	endif
#endif

#endif
