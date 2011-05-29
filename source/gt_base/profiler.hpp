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

//--------------------------------------------------------
//!\class	cProfiler
//!\brief
class cProfiler{
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
	typedef std::map<const char*,  dLineToNameMap> dFileTo2ndMap;

	dEntryMap mEntries;	//! \note A map is slightly slower than a vector, but tokens are always
	dEntryMap::iterator scrEntryItr;

	//- This helps us quickly determine if we need to make a new entry or not.
	dFileTo2ndMap mEntryLookup;
	dFileTo2ndMap::iterator scrLUp1stItr;
	dLineToNameMap::iterator scrLUp2ndItr;

public:
	class cToken{
	private:
		cToken& operator=(cToken& pToken){ DUMB_REF_ARG(pToken); return *this; }

	public:
		const dNameHash		mEntryID;	//!< \note Can't use iterators because the vector changes all the time.
		const dMillisec		mTimeStarted;

		cProfiler*		mProfiler;	//!< When the parent is destroyed, it has to inform remaining tokens and it does this by setting mProfiler to null.

		cToken(cProfiler* pParent, dNameHash pID, dMillisec pTime);
		~cToken();
	};

	std::set<cToken*> mActiveTokens;	//!< Allows you to inform a token when it's parent has been cleaned up.
	dMillisec (*mGetTime)();

	cProfiler();
	~cProfiler();

	cToken& makeToken(const dNatChar* pFile, const unsigned int pLine);
	void tokenFinished(cProfiler::cToken* pToken);
	void flushThatLog(std::ostream &log);
	cProfiler& operator += (const cProfiler& pCopyIt);
};

/*
#ifdef DEBUG
	cCoolStatic<cProfiler> gProfiler;

	#define PROFILE cProfiler::cToken profileToken = cCoolStatic<cProfiler>::get()->makeToken(__FILE__, __LINE__)
	#define LOG_PROFILE cCoolStatic<cProfiler>::get()->flushThatLog(std::cout);
#else
	#define PROFILE
	#define LOG_PROFILE
#endif
*/

#endif
