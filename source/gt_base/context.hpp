/*
 * !\file	context.hpp
 * !\brief	The context and multithreaded figment provide mechanisms for figments to handle multi threading. In this regard,
 * !		the design of the context drives the strategy for multithreading figments.
 * !		The exact objectives are:
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
 *  Copyright (C) 2010  Stuart Bridgens
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
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

	typedef std::deque<cFigContext*> dProgramStack;	//!< This is the entire stack of figments. The pancake map below doesn't copy this stack (in case you're wondering).
}

///////////////////////////////////////////////////////////////////////////////////
// errors
namespace excep{
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
	//!\brief	A context is used to avoid circular references and to also create a way to
	//!			block threads from running into each other.
	//!\note	Things get a little confusing when you copy another figment. These other
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
		dProgramStack makeStackDump();				//!< Spits out a copy of the program stack.
		ptrFig getFirstOfType(dNameHash aType);		//!< Needs to change! Scans the stack and returns the first figment of the type we're looking for. If it didn't find one, it returns an empty figment. \note is slow because we want all the operations to be as fast as possible.
		dConSig getSig() const;						//!< Get unique signature.
		void runJackJobs();	//!< Runs through the list of jobs for jacking. Should do this at the start of a loop. If you use a fake context as part of a unit test, consider running this function after you do any jacking.
		void addJackJob(ptrLead aLead, ptrFig aTarget);	//!< If your figment is going to call jack from a run, it must add it as a jack job.

	protected:
		void add(cFigContext *pFig);					//!< Adds a figment reference to the stack.
		void finished(cFigContext *pFig);				//!< Removes a figment reference from the stack.

		friend class cFigContext;

	private:

		struct sInfo{
			unsigned int timesStacked;
			dNameHash realID;	//!< Lets you determine what sort of class is on the stack.
			bool fromOtherStack;	//!< We don't want to force stop figments that came from another stack.

			sInfo(unsigned int pTime, dNameHash pID) : timesStacked(pTime), realID(pID), fromOtherStack(false) {}
			sInfo() : timesStacked(0), realID(0), fromOtherStack(false) {}
		};

		struct sJackJob{
			ptrLead mLead;
			ptrFig mTarget;
		};

		typedef std::map<const cFigContext*, sInfo> dMapInfo;		//!<
		typedef std::list<sJackJob> dJackJobs;

		#ifdef GT_THREADS
			const dThreadID mThreadID;
		#endif

		bool mKeepPopping;
		dConSig mSig;	//!<
		dProgramStack mStack;	//!< This is the entire stack of figments in the order that they were added in.
		dMapInfo mSigInfo;	//!< Stores more info about different items on the stack.
		dJackJobs mJobs;	//!< Favour list building speed over iteration speed.

		dMapInfo::iterator itrInfo;
	};

	//-------------------------------------------------------------------------------------
	//!\brief	Provides services to handle multithreading figments.
	class cFigContext : public iFigment{
	public:
		cFigContext();
		virtual ~cFigContext();

		void start(cContext *con);	//!<	Puts this figment onto the given context stack, but only if it's not already using a context.
		void stop(cContext *con);	//!<	Takes the figment off the stack.

	protected:
		cContext *currentCon;	//!< This allows a thread to check this figment to see if it already has a context, and if it's blocked or not.

		void emergencyStop();	//!<	Used by the context a forcibly removing figments from itself.

		friend class cContext;
		friend class cBlueprint;

	private:

		#ifdef GT_THREADS
			//boost::condition_variable conSync;
			boost::mutex conMu;
			//boost::unique_lock<boost::mutex> lock;
		#endif
	};
}



#endif
