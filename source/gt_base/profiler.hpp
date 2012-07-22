/*
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

#ifndef	PROFILER_HPP
#define	PROFILER_HPP

#include "utils.hpp"
#include "gt_string.hpp"

#include <iostream>
#include <sstream>
#include <vector>
#include <map>
#include <set>

#include <stdio.h>
#include <string.h>
#include <stdarg.h>

#ifdef GT_THREAD
	#include <boost/thread/locks.hpp>
	#include <boost/thread.hpp>

	typedef boost::unique_lock<boost::mutex> dProfileLock;

	#define CRITLOCK dProfileLock critSection(mu)
#else
	#define CRITLOCK
#endif

//----------------------------------------------------------------------------------------------------------------
//!\class		cProfiler
//!\brief		Manages profiling tokens which track how long they are alive for. Can give
//!				you info about how long different bits of code took to run.
//!note		The profiler is intended to be thread safe only if its container is accessed
//!				in a thread safe manner. However, because a token can die at any point,
//!				some effort has been made to ensure it's done in a thread safe manner.
class cProfiler{
public:
	class cToken{
	public:
		const dNameHash	mEntryID;	//!< \note Can't use iterators because the vector changes all the time.
		const dMillisec			mTimeStarted;

		cToken(cProfiler *pParent, dNameHash pID, dMillisec pTime);
		cToken(const cToken &);
		~cToken();

	protected:
		void profilerDied();	//!< The linked profiler calls this if this token is still active when the profiler dies.

		friend class cProfiler;

	private:
		cProfiler *mProfiler;	//!< When the parent is destroyed, it has to inform remaining tokens and it does this by setting mProfiler to null.

		cToken& operator=(cToken& pToken){ DUMB_REF_ARG(pToken); return *this; }	//!< Banned.
	};

	std::set<cToken*> mActiveTokens;	//!< Allows you to inform a token when it's parent has been cleaned up.
	dMillisec (*mGetTime)();

	cProfiler();
	~cProfiler();

	cToken makeToken(const dPlaChar *pFile, const unsigned int pLine);
	void tokenFinished(cProfiler::cToken *pToken);
	void flushThatLog(std::ostream &log);
	cProfiler& operator += (const cProfiler &pCopyIt);

private:
	class cEntry{
	public:
		dStr mInfo;
		dMillisec mTimesProfiled;
		dMillisec mAveTime;
		dMillisec mMaxTime;

		cEntry();
		~cEntry();
	};

	typedef std::map<dNameHash, cEntry> dEntryMap;
	typedef std::map<unsigned int, dNameHash> dLineToNameMap;
	typedef std::map<const dPlaChar*,  dLineToNameMap> dFileTo2ndMap;

	dEntryMap mEntries;	//! \note A map is slightly slower than a vector, but tokens are always
	dEntryMap::iterator scrEntryItr;

	//- This helps us quickly determine if we need to make a new entry or not.
	dFileTo2ndMap mEntryLookup;
	dFileTo2ndMap::iterator scrLUp1stItr;
	dLineToNameMap::iterator scrLUp2ndItr;

#ifdef GT_THREAD
	boost::mutex	mu;
#endif
};


#endif
