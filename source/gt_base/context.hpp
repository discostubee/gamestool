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
 *
 * Outline of how plugs works:
 * Using a leads 'get plug' function returns a shadow plug. A shadow is unique for every context, and because every thread has a different context, threading
 * shouldn't be a problem. Shadows can either be updated from the original, or the original is overwritten by the shadow depending on what the shadow has
 * logged its use as. Plugs store their own shadows, so a figment can update their shadows every time they run. running the update causes the context that
 * shadow belongs to to become locked. Once a context becomes locked it can't be used to use any more plugs until it unlocks.
 *
 *
 */

#ifndef CONTEXT_HPP
#define CONTEXT_HPP

#include "world.hpp"

#include <deque>

///////////////////////////////////////////////////////////////////////////////////
// typedefs
namespace gt{
	typedef iFigment* dFigConSig;						//!< A figments context signature. Because this value doesn't need to be saved/loaded it should be fine whatever the byte size is.
	typedef std::deque<dFigConSig> dProgramStack;	//!< This is the entire stack of figments. The pancake map below doesn't copy this stack (in case you're wondering).

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
	class cContext{
	public:
		dConSig mSig;	//!< try not to modify it.

		cContext();
		cContext(const cContext & copyMe);
		~cContext();

		void add(dFigConSig pFig, dNameHash pClassID);				//!< Adds a figment reference to the stack.
		void finished(dFigConSig pFig);								//!< Removes a figment reference from the stack.
		bool isStacked(dFigConSig pFig, dNameHash pClassID = 0);	//!< Determines if the figment is stacked. It is optional to check the ID type as well.
		dProgramStack makeStackDump();								//!< Spits out a copy of the program stack.

	private:

		struct sInfo{
			unsigned int timesStacked;
			dNameHash realID;	//!< Lets you determine what sort of class is on the stack.

			sInfo(unsigned int pTime, dNameHash pID) : timesStacked(pTime), realID(pID) {}
			sInfo() : timesStacked(0), realID(0) {}
		};

		typedef std::map<dFigConSig, sInfo> dMapInfo;		//!<

		#ifdef GT_THREADS
			const dThreadID mThreadID;
		#endif

		dProgramStack mStack;	//!< This is the entire stack of figments in the order that they were added in.
		dMapInfo mSigInfo;	//!< Stores more info about different items on the stack.

		dMapInfo::iterator itrInfo;
	};

	//-------------------------------------------------------------------------------------
	//!\brief	Provides services to handle multithreading figments.
	class cFigContext : public iFigment{
	public:
		cFigContext();
		virtual ~cFigContext();

	protected:
		cContext *currentCon;	//!< This allows a thread to check this figment to see if it already has a context, and if it's blocked or not.

		//!\brief	Puts this figment onto the stack.
		void start(cContext *con);

		//!\brief
		void stop(cContext *con);

		//!\brief	Kills a zombie figment. Ensure this is only called when clearing the stack of zombies. Brains!
		void kill();

	private:

		#ifdef GT_THREADS
			//boost::condition_variable conSync;
			boost::mutex conMu;
			//boost::unique_lock<boost::mutex> lock;
		#endif
	};
}



#endif
