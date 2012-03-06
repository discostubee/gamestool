#include "profiler.hpp"

////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////

cProfiler::cEntry::cEntry():
	mInfo(""),
	mTimesProfiled(0),
	mAveTime(0),
	mMaxTime(0)
{}

cProfiler::cEntry::~cEntry(){
}


cProfiler::cProfiler():
	mGetTime(NULL)
{}

cProfiler::~cProfiler(){
	for(	std::set<cToken*>::iterator i = mActiveTokens.begin();
			i != mActiveTokens.end();
			++i
	){
		(*i)->profilerDied();
	}
}

cProfiler::cToken
cProfiler::makeToken(const dPlaChar* pFile, const unsigned int pLine){
	CRITLOCK;

	dNameHash tempHash = 0;

	//- Using an ugly map-of-a-map to quickly find out if we need to add a new entry or not.
	scrLUp1stItr = mEntryLookup.find(pFile);
	if(scrLUp1stItr != mEntryLookup.end()){
		scrLUp2ndItr = scrLUp1stItr->second.find(pLine);
	}

	if(scrLUp1stItr == mEntryLookup.end() ||  scrLUp2ndItr == scrLUp1stItr->second.end()){ //Need to add new entry first
		std::stringstream tempS;
		tempS << pFile << ", " << pLine;
		tempHash = makeHash( tempS.str().c_str() );
		mEntries[tempHash].mInfo = tempS.str();
	}else{
		tempHash = scrLUp2ndItr->second;
	}

	if(mGetTime != NULL){
		return cToken( this, tempHash, mGetTime() );
	}else{
		return cToken( this, tempHash, 0 );
	}
}

void
cProfiler::tokenFinished(cProfiler::cToken* pToken){
	CRITLOCK;

	dMillisec totalTime = 0;
	cEntry* i = &mEntries[pToken->mEntryID];

	if(mGetTime != NULL && pToken->mTimeStarted != 0){
		totalTime = mGetTime() - pToken->mTimeStarted;
		i->mAveTime = (
			( i->mAveTime * i->mTimesProfiled ) + totalTime
		) / ( i->mTimesProfiled +1 );

		++i->mTimesProfiled;

		if(i->mMaxTime < totalTime){
			i->mMaxTime = totalTime;
		}

	}else{
		++i->mTimesProfiled;
	}

	{	// below should be redundant. It should ALWAYS find this token.
		std::set<cToken*>::iterator temp = mActiveTokens.find(pToken);
		if( temp != mActiveTokens.end() )
			mActiveTokens.erase( temp );
	}
	//mActiveTokens.erase( mActiveTokens.find(pToken) );
}

void
cProfiler::flushThatLog(std::ostream &log){
	CRITLOCK;

	log << "File: Line: Times profiled: Average time: Max time:" << std::endl;

	for(
		dEntryMap::iterator i = mEntries.begin();
		i != mEntries.end();
		++i
	){
		log
			<< i->second.mInfo << ": "
			<< i->second.mTimesProfiled << ": "
			<< i->second.mAveTime << ": "
			<< i->second.mMaxTime << std::endl;
	}
	mEntries.clear();
	mEntryLookup.clear();
}

cProfiler&
cProfiler::operator += (const cProfiler& pCopyIt){
	CRITLOCK;

	if(&pCopyIt == this)
		return *this;

	mEntries.insert(pCopyIt.mEntries.begin(), pCopyIt.mEntries.end());
	return *this;
}

cProfiler::cToken::cToken(cProfiler* pParent, dNameHash pEntry, dMillisec pTime):
	mEntryID(pEntry),
	mTimeStarted(pTime),
	mProfiler(pParent)
{
	//- Only do this when the token is copied.
	//mProfiler->mActiveTokens.insert(this);
}

cProfiler::cToken::cToken(const cToken &copyMe):
		mEntryID(copyMe.mEntryID),
		mTimeStarted(copyMe.mTimeStarted),
		mProfiler(copyMe.mProfiler)
{
	mProfiler->mActiveTokens.insert(this);	// put here instead of inside cProfiler because the pointer needs to be valid.
}

cProfiler::cToken::~cToken(){
	CRITLOCK;

	if(mProfiler != NULL){
		mProfiler->tokenFinished(this);
	}
}

void cProfiler::cToken::profilerDied(){
#ifdef GT_THREAD
	if(!mu.try_lock())
		return;

	mProfiler = NULL;
	mu.unlock();
#else
	mProfiler = NULL;
#endif

}
