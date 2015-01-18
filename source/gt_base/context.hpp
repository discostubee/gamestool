/*
 * !\file	context.hpp
 * !\brief	The exact objectives are:
 * !		- Prevent circular flow, as soon as a run command encounters itself in the context it throws.
 * !		- Do the same for running as we do for jacking. Jacking uses a different context to running.
 * !		- Cause a thread to wait if it tries to run or jack into a figment that is currently working with another thread.
 * !		- Unwind if a figment tries to run or jack into another that is currently waiting. In other words, prevent deadlocks.
 *
 * !\todo	Need to optimise both the context and the way it's used by figment plugs. Basically, the figment plug should have 2 special functions
 * !		for nunning and jacking. These 2 functions inform the context how it's setup, and if that plug changes at all, the context or contexts
 * !		are updated. That way, the context map is not built every we time we start and stop using a context. We need to still start and stop
 * !		a context, so that we can tell if we are going to collide with ourselves.
 *
 * !\todo	Implement the zombie purge after something throws during a run. We should never throw out of a jack.
 *
 * !\note	Outline of how plugs works:
 * !		Using a leads 'get plug' function returns a shadow plug. A shadow is unique for every context, and because every thread has a different
 * !		context, threading shouldn't be a problem. Shadows can either be updated from the original, or the original is overwritten by the
 * !		shadow depending on what the shadow has logged its use as. Plugs store their own shadows, so a figment can update their shadows every
 * !		time they run. running the update causes the context that shadow belongs to to become locked.
 *
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

#ifndef CONTEXT_HPP
#define CONTEXT_HPP

#include "world.hpp"

#include <deque>

///////////////////////////////////////////////////////////////////////////////////
// typedefs and forward decs
namespace gt{
	class cFigContext;

	typedef std::deque<cFigContext*> dProgramStack;	//!< This is the entire stack of figments.
}

///////////////////////////////////////////////////////////////////////////////////
// errors
namespace excep{
	class badContext : public base_error{
	public:
		badContext(const char* pFile, const unsigned int pLine) :
			base_error(pFile, pLine)
		{ addInfo("bad context"); }

		virtual ~badContext() throw() {}
	};

	class stackFault: public base_error{
	public:
		stackFault(
			gt::dProgramStack pBadStack,
			const std::string &pMoreInfo,
			const char* pFile,
			unsigned int pLine
		);
		virtual ~stackFault() throw();
	};

	class stackFault_selfReference: public stackFault{
	public:
		stackFault_selfReference(
			gt::dProgramStack pBadStack,
			const char* pFile,
			unsigned int pLine
		) : stackFault(pBadStack, "self reference", pFile, pLine){
		}

		virtual ~stackFault_selfReference() throw() {}
	};
}

///////////////////////////////////////////////////////////////////////////////////
// classes
namespace gt{

	//-------------------------------------------------------------------------------------
	//!\brief	A context is used to both provide contextual data (IE get data from things
	//!			before me), and to prevent bad situations (circular references, thread
	//!			collisions).
	//!\note	Things get a little confusing when you copy another context.
	//!			because when running a thread, the other context can finish with its figments
	//!			and remove them. We still want to keep this stack because we still want to
	//!			prevent circular references, we just don't want to force stop them when our
	//!			context ends.
	class cContext{
	public:
		cContext();
		cContext(const cContext & copyMe);
		~cContext();

		bool isStacked(cFigContext *pFig);			//!< Determines if the figment is stacked.
		bool canStack(cFigContext *pFig);			//!< Determines if the figment can be stacked.
		dProgramStack makeStackDump();				//!< Spits out a copy of the program stack.
		ptrFig getFirstOfType(dNameHash aType);		//!< Needs to change! Scans the stack and returns the first figment of the type we're looking for. If it didn't find one, it returns an empty figment.
		dConSig getSig() const;						//!< Get unique signature.
		void startJackMode();						//!< Sets the context into jack mode, where isStacked is only true if a figment was added during this mode. The mode is reset once the stack is unwound back to the point where the mode was engaged.

		cContext& operator=(const cContext &pCon);

	protected:
		void add(cFigContext *pFig);					//!< Adds a figment reference to the stack.
		void finished(cFigContext *pFig);				//!< Removes a figment reference from the stack.

		friend class cFigContext;

	private:
		static const int ORIGINAL = -1;	//!< Is this context an original and not a copy. used when preventing an unwind into another context's stack.
		static const int NOT_JACKING = -1;

		struct sStackInfo{
			enum eMode{ eModeRun, eModeJack, eModeRestacked };

			const dNameHash figType;	//!< Lets you determine what sort of figment is on the stack.

			eMode mCurMode;	//!< In order to allow figments to be restacked in jack mode, we need to know more about when they were stacked.

			sStackInfo(dNameHash pType, eMode pMode) : figType(pType), mCurMode(pMode) {}
		};

		typedef std::map<const cFigContext*, sStackInfo> dMapInfo;		//!<

		bool mKeepPopping;
		dConSig mSig;	//!<
		dProgramStack mStack;	//!< This is the entire stack of figments in the order that they were added in.
		dMapInfo mStackInfo;	//!< Stores more info about different items on the stack.
		int mCopyIdx;	//!< Index of the last element copied from another context. -1 means nothing was copied. used when preventing an unwind into another context's stack.
		int mJackModeIdx;	//!< The index at which jackmode was engaged.

		dMapInfo::iterator itrInfo;	//!< scratch space.
	};

	//-------------------------------------------------------------------------------------
	//!\brief	Figments use contexts so that they can access other figments upstream, and
	//!			to prevent circular references and thread collisions.
	class cFigContext : public iFigment{
	public:
		cFigContext();
		virtual ~cFigContext();

		virtual const dPlaChar* name() const { return "fig context"; }	//!< Need this when stack dumping from destructor.

		void start(cContext *con);	//!< If threaded, this figment is locked. Puts this figment onto the given context stack, but only if that figment isn't already stacked. Updates all plugs in the roster.
		void stop(cContext *con);	//!< If threaded, this figment is unlocked. Takes the figment off the stack.
		void addUpdRoster(cBase_plug *pPlug);	//!< Adds a plug to the list of things to update when we start. Typically you can use this in the constructor. NOT threadsafe.
		void remFromRoster(cBase_plug *pPlug);	//!< Used only if a plug is removed from a class during its jack/run functions. NOT threadsafe.

	protected:
		cContext *currentCon;	//!< This allows a thread to check this figment to see if it already has a context, and if it's blocked or not.

		void emergencyStop();	//!< If there is a problem, we want the figment to be able to stop using a context.

		//- It is hoped that the compiler will reduce the cost to call these to 0 if we use a un-threaded version.
		void updatePlugs();	//!< Update all the plug shadows.

	friend class cContext;
	friend class cBlueprint;

	private:
		bool locked;

		#ifdef GT_THREADS

			typedef boost::lock_guard<boost::recursive_mutex> dLock;

			boost::recursive_mutex muCon;	//!< Mutex for start and stop.
			std::vector<cBase_plug*> updateRoster;	//!< Reference to plugs that need to update. DO NOT delete the contents, even on destruction.
			std::vector<cBase_plug*>::iterator itrRos;
			bool updating;
		#endif
	};
}



#endif
