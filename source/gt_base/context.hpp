/*
 * !\file	context.hpp
 * !\brief	The context and multithreaded figment provide mechanisms for figments to handle multi threading. In this regard,
 * !		the design of the context drives the strategy for multithreading figments.
 * !		The exact objectives are:
 * !		- Prevent circular runs as soon as a run command encounters itself in the context.
 * !		- Do the same as running as we do for jacking. Jacking uses a different context to running.
 * !		- Cause a thread to wait if it tries to run or jack into a figment that is currently working with another thread.
 * !		- Unwind if a figment tries to run or jack into another that is currently waiting. In other words, prevent deadlocks.
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
 *
 * EXAMPLE A
 * Dealt with because when c encounters c again, it unwinds. b gets blocked
 * until c winds.
 *
 *	    a
 * 	    b-->c
 * 	    b   c
 * 	|-->0   c
 * 	|   c<--c
 *  |<--c
 *
 * EXAMPLE B
 *
 *  a
 *  b-->c
 *  b   d-->e
 *  b   0   e--|
 *  b          |
 *  f-->0      |
 *  0   g<-----|
 *  0   g
 *
 */

#ifndef CONTEXT_HPP
#define CONTEXT_HPP

#include "world.hpp"

#include <deque>

///////////////////////////////////////////////////////////////////////////////////
namespace gt{
	class cFigContext;
}

///////////////////////////////////////////////////////////////////////////////////
// typedefs
namespace gt{
	typedef cFigContext* dFigConSig;						//!< A figments context signature.
	typedef std::deque<dFigConSig> dProgramStack;			//!< This is the entire stack of figments. The pancake map below doesn't copy this stack (in case you're wondering).
	typedef std::map<dFigConSig, int> dFigSigCount;			//!<
	typedef std::map<dNameHash, dProgramStack> dPancakes;	//!< There are many different kinds of pancakes, and each plate can have any number of that kind on it.
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
		): base_error(pFile, pLine){
			addInfo( dStr("stack fault: ") + pMoreInfo );
		}
		virtual ~stackFault() throw() {}
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
	class cContext{
	public:
		cContext();
		cContext(const cContext & copyMe);
		~cContext();

		void add(dFigConSig pFig);				//!< Adds a figment reference to the stack.
		void finished(dFigConSig pFig);			//!< Removes a figment reference from the stack. You can try to remove the same figment a few times without problem.
		bool isStacked(dFigConSig pFig);		//!< Determines if the figment is stacked.
		refFig getLastOfType(dNameHash pType);
		dProgramStack makeStackDump();			//!< Spits out a copy of the program stack.
		bool isBlocked();

	private:
		#ifdef GT_THREADS
			const dThreadID mThreadID;
		#endif

		dProgramStack mStack;		//!< This is the entire stack of figments in the order that they were added in.
		dFigSigCount mTimesStacked;	//!<
		dPancakes mPlateOPancakes;	//!< Maps types to a stack, so you can tell what the most recent object of a certain type is.

		dFigSigCount::iterator figSigItr;
		dPancakes::iterator cakeItr;
	};

	//-------------------------------------------------------------------------------------
	//!\brief	Provides services to handle multithreading figments.
	class cFigContext : public iFigment{
	public:
		cFigContext();
		~cFigContext();

	protected:

		//!\brief	Puts this figment onto the stack. If this thread is already on the stack,
		//!			if it's already in use by another thread, stackFault_selfReference is thrown.
		//!			If this figment is in use by another thread, it is blocked and wait for it to be free. But if
		//!			the context is already blocked, something is thrown to avoid deadlocks.
		//!\todo	avoid using exceptions.
		void start(cContext *con);
		void stop(cContext *con);

	private:
		cContext *currentCon;	//!< This allows a thread to check this figment to see if it already has a context, and if it's blocked or not.

		#ifdef GT_THREADS
			boost::condition_variable conSync;
			boost::mutex conMu;
		#endif
	};
}



#endif
