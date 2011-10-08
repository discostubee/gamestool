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
	public:
		static const cCommand::dUID	xAdd;

		cRunList();
		virtual ~cRunList();

		//- Required
		static const char* identify(){ return "run list"; }
		virtual const char* name() const{ return identify(); }
		virtual dNameHash hash() const{ return tOutline<cRunList>::hash(); }

		//- Optional
		virtual void run(cContext* pCon);				//!< runs every element in the list
		virtual void save(cByteBuffer* pAddHere);
		virtual void loadEat(cByteBuffer* pBuff, dReloadMap* pReloads);
		virtual void getLinks(std::list<ptrFig>* pOutLinks);

	protected:
		typedef std::vector< tPlug<ptrFig> > dList;
		typedef dList::iterator dListItr;

		dList mList;

		void patAdd(ptrLead aLead);
	};

	//-----------------------------------------------------------------------------------------------
	//!\brief Running or not running an object is controlled by this valve station (still keeping
	//		that plumbing analogy going). Use the jack to change what objects will be run every
	//		time this figment is run.
	class cValves: public cRunList, private tOutline<cValves>{
	protected:
		std::map< dListItr, tPlug<bool> > mStates;	//!< Maps states to the list entries. These states control which objects are run.

	public:
		static const cPlugTag*	xPT_state;		//!< Turn turns valve on.
		static const cPlugTag*	xPT_valveIdx;	//!< The numeric index for the valve.

		static const cCommand::dUID	xSetState;

		cValves();
		virtual ~cValves();

		//- Required
		static const char* identify(){ return "valve station"; }
		virtual const char* name() const { return identify(); }
		virtual dNameHash hash() const { return tOutline<cValves>::hash(); }

		//- Optional
		virtual void run(cContext* pCon);				//!< runs every element in the list
		virtual void save(cByteBuffer* pAddHere);
		virtual void loadEat(cByteBuffer* pBuff, dReloadMap* pReloads);

		static dNameHash extends(){ return getHash<cRunList>(); }
		virtual dNameHash getExtension() const { return extends(); }

	protected:
		void patSetValve(ptrLead pLead);
	};
}

#endif
