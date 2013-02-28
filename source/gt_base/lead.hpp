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
// Forward dec.
namespace gt{
	template<typename A> class tPlug;

#	ifdef GUTU
		void startLead(ptrLead, dConSig);
		void stopLead(ptrLead);
		void startLead(cLead &, dConSig);
		void stopLead(cLead &);
#	endif
}

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
	//!\note	The interface is all threadsafe because an unplug call can come at any time.
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

		//!\brief	Sets the value of the plug identified by the tag.
		template<typename CONTAIN> void setValue( const CONTAIN *input, const cPlugTag *tag);

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

		//!\brief
		template<typename TYPE> void getPileValue(std::vector< TYPE > *target);

		//!\brief	Clears out the pile. Provided for those times when instead of making a new lead, you want to keep
		//!			the tagged data.
		void clearPile();

		//!\brief	When a plug dies, it must let the lead know it is no longer valid.
		void unplug(cBase_plug* pPlug);

	protected:
		typedef std::map<cPlugTag::dUID, cBase_plug*> dDataMap;
		typedef std::list<cBase_plug*> dPiledData;
		
#		ifdef GT_THREADS
			typedef boost::lock_guard<boost::recursive_mutex> dLock;

			boost::recursive_mutex mu;
			dConSig mCurrentSig;

			void start(dConSig pSig);
			void stop();

#			ifdef GTUT
				friend void startLead(ptrLead, dConSig);
				friend void stopLead(ptrLead);
				friend void startLead(cLead &, dConSig);
				friend void stopLead(cLead &);
#			endif
#		endif

		dDataMap mTaggedData; 	//!<
		dPiledData mDataPile;	//!<

		dDataMap::iterator scrTDataItr;
		dPiledData::iterator scrPDataItr;

	friend class cFigment;

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

	template<typename CONTAIN>
	void
	cLead::setValue( const CONTAIN *input, const cPlugTag *tag){
		scrTDataItr = mTaggedData.find(tag->mID);

		if(scrTDataItr == mTaggedData.end())
			return;

		scrTDataItr->second->copyFrom(input);
	}

  	template<typename PLUG_TYPE>
	void
	cLead::getPile(std::vector< tPlug<PLUG_TYPE> > *target){
		target->reserve(target->size() + mDataPile.size());
		for(scrPDataItr = mDataPile.begin(); scrPDataItr != mDataPile.end(); ++scrPDataItr){
#			ifdef GT_THREADS
				target->push_back( tPlug<PLUG_TYPE>() );
				(*scrPDataItr)->readShadow( &target->back(), mCurrentSig );
#			else
				 target->push_back( (*scrPDataItr) );
#			endif
		}
	}

  	template<typename TYPE>
  	void
	cLead::getPileValue(std::vector< TYPE > *target){
  		TYPE valBase;
		target->reserve(target->size() + mDataPile.size());
#		ifdef GT_THREADS
			tPlug<TYPE> passThrough;
#		endif

		for(scrPDataItr = mDataPile.begin(); scrPDataItr != mDataPile.end(); ++scrPDataItr){
			target->push_back(valBase);
#			ifdef GT_THREADS
				(*scrPDataItr)->readShadow( &passThrough, mCurrentSig );
				target->back() = passThrough.get();
#			else
				(*scrPDataItr)->copyInto( &target->back() );
#			endif
		}
  	}

}

///////////////////////////////////////////////////////////////////////////////////
// Unit testing
#ifdef GTUT

namespace gt{
	//- In a couple of unit tests we need to emulate how leads are stopped and started in jack and run mode.
	void startLead(ptrLead lead, dConSig pSig);
	void stopLead(ptrLead lead);
	void startLead(cLead &lead, dConSig sig);
	void stopLead(cLead &lead);
}

#endif


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

#endif
