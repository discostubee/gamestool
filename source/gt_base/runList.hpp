/*
 * !\file	runList.hpp
 * !\brief	contains the runlist declaration.
 */

#ifndef RUNLIST_HPP
#define RUNLIST_HPP

#include <list>
#include "figment.hpp"

namespace gt{ //gamestool

	//-----------------------------------------------------------------------------------------------
	//!\brief	When you run this figment, you also run every figment in its list.
	class cRunList: public cFigment, private tOutline<cRunList>{
	protected:
		typedef std::vector< cPlug<ptrFig> > dList;
		typedef dList::iterator dListItr;

		dList mList;

	public:
		static const cCommand*	xAdd;

		enum{
			eAdd = cFigment::eSwitchEnd,
			eSwitchEnd,
		};

		static const dNatChar* identify(){ return "run list"; }
		static void draft(){ tOutline<cRunList>::draft(); }
		static void requirements();

		cRunList();
		virtual ~cRunList();

		virtual const dNatChar* name() const{ return identify(); }
		virtual dNameHash hash() const{ return tOutline<cRunList>::hash(); }

		virtual void run(cContext* pCon);				//!< runs every element in the list
		virtual void jack(ptrLead pLead);
		virtual cByteBuffer& save();
		virtual void loadEat(cByteBuffer* pBuff, dReloadMap* pReloads);
		virtual void getLinks(std::list<ptrFig>* pOutLinks);
	};

	//-----------------------------------------------------------------------------------------------
	//!\brief Running or not running an object is controlled by this valve station (still keeping
	//		that plumbing analogy going). Use the jack to change what objects will be run every
	//		time this figment is run.
	class cValves: public cRunList, private tOutline<cValves>{
	protected:
		std::map< dListItr, cPlug<bool> > mStates;	//!< Maps states to the list entries. These states control which objects are run.

	public:
		static const cPlugTag*	xPT_state;		//!< Turn turns valve on.
		static const cPlugTag*	xPT_valveIdx;	//!< The numeric index for the valve.

		static const cCommand*	xSetState;

		enum{
			eSetState = cRunList::eSwitchEnd +1,
			eSwitchEnd
		};

		static const dNatChar* identify(){ return "run list"; }
		static const void draft(){ tOutline<cValves>::draft(); }

		cValves();
		virtual ~cValves();

		virtual const dNatChar* name() const { return identify(); }
		virtual dNameHash hash() const { return tOutline<cValves>::hash(); }

		virtual void run(cContext* pCon);				//!< runs every element in the list
		virtual void jack(ptrLead pLead);
		virtual cByteBuffer& save();
		virtual void loadEat(cByteBuffer* pBuff, dReloadMap* pReloads);
	};
}

#endif
