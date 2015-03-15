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

#ifdef GT_THREADS
#	include "threadTools.hpp"
#endif

#include <boost/smart_ptr.hpp>

///////////////////////////////////////////////////////////////////////////////////
// Forward dec.
namespace gt{

#	ifdef GUTU
		void startLead(ptrLead, dConSig);	//!< In order to use a lead inside a unit tests, we need to treat it like it was jacked.
		void stopLead(ptrLead);				//!< End with this.
		void startLead(cLead &, dConSig);	//!< Handy if you're not using the usual smart pointer.
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
	//!\note	The interface is all thread-safe because an unplug call can come at any time.
	//!\note	Leads can not be contained by a plug.
	//!\note	A lot of assignment methods are not constant because of threading.
	class cLead{
	public:
		typedef tSpitLemming<cBase_plug>::tLemming dRef;

		const cCommand::dUID mCom;	//!< The command for this lead.

		cLead(cCommand::dUID aCom);
		cLead(const cLead &otherLead);
		~cLead();

		//!\brief	Links a tagged reference to a plug to our lead.
		void linkPlug(cBase_plug *linkMe, const cPlugTag *pTag);

		//!\brief	Get a plug from a lead by copying its value into another plug.
		bool copyPlug(cBase_plug *setMe, const cPlugTag* pTag);

		//!\brief	If it has the tagged plug, it sets it to the value stored in the plug being passed in.
		bool setPlug(const cBase_plug *copyMe, const cPlugTag *pTag);

		//!\brief	If it has the tagged plug, it appends the given input plug.
		bool plugAppends(cBase_plug *addFrom,  const cPlugTag *pTag);

		//!\brief	If it has the tagged plug, it appends to the input (using the += operator).
		bool appendPlug(cBase_plug *addTo, const cPlugTag *pTag);

		//!\brief	If you want to pass a plug from one lead to another, here's how you do it.
		//!\param	passTo	This is the lead that will get the plug from this lead.
		//!\param	pGetTag	This is the tag used to find the plug in this lead.
		//!\param	pPutTag	(optional) If specified, the target lead will be assigned the plug using this
		//!				put tag instead
		//!\return	false if the plug wasn't found using the get tag.
		bool passPlug(cLead *passTo, const cPlugTag *pGetTag, const cPlugTag *pPutTag = NULL);

		//!\brief	If you don't want to re-create a lead without this plug, or you don't want to over-ride it, you
		//!			can remove it with this.
		void remPlug(const cPlugTag *pGetTag);

		//!\brief	When a plug dies, it must let the lead know it is no longer valid.
		void unplug(cBase_plug* pPlug);

		bool has(const cPlugTag *pTag);

	protected:
		typedef std::map<cPlugTag::dUID, cBase_plug*> dDataMap;
		
		dDataMap mTaggedData;
		dDataMap::iterator scrItr;

		//-
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

	friend class cFigment;

	private:
		cLead& operator = (const cLead &aOtherLead);	//!< Banned

	};
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


#endif
