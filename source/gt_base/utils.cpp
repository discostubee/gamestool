/*
 * utils.cpp
 *
 *  Created on: 08/07/2010
 *      Author: stuandlou
 */

#include "utils.hpp"

dNameHash 
makeHash(const dNatChar* pString)
{
	dNameHash hash = 0;
	size_t len = natCharLen(pString);

	if(len == 0)
		return 0;

	dNatChar* hashMe = new dNatChar[len];

	for(size_t utfIdx = 0; utfIdx < len; ++utfIdx)
		hash = ((hash << 5) + hash) ^ hashMe[utfIdx];

	delete [] hashMe;

	return hash;
}


////////////////////////////////////////////////////////////
cTracker::dItemMap* 			cTracker::xObjectsActive;	// Don't assign here.
cTracker::dItemMap::iterator	cTracker::xItemItr;			// Don't assign here.
dNameHash						cTracker::xTempHash;
cTracker::cAutoCleanup 			cTracker::xCleanup;

cTracker::cAutoCleanup::~cAutoCleanup(){
	cTracker::ensureSetup(false);
}

void cTracker::ensureSetup(bool setup){
	static bool objectsActiveSetup = false;
	if(setup){
		if(!objectsActiveSetup){
			xObjectsActive = new dItemMap();
			objectsActiveSetup = true;
		}
	}else{
		if(objectsActiveSetup){
			delete xObjectsActive;
			xObjectsActive = NULL;
			objectsActiveSetup = true;
		}
	}
}

void
cTracker::start(const char* pName){
	ensureSetup();

	xTempHash = makeHash(pName);
	xItemItr = xObjectsActive->find(xTempHash);

	if( xItemItr == xObjectsActive->end() ){
		sItem newItem = {dStr(pName), 1};
		xObjectsActive->insert( dItemPair(xTempHash, newItem) );
	}else{
		++xItemItr->second.mCount;
	}
}

void
cTracker::stop(const char* pName){
	ensureSetup();

	xTempHash = makeHash(pName);
	--(*xObjectsActive)[ xTempHash ].mCount;
}

void
cTracker::makeReport(std::ostream &report){
	ensureSetup();

	report << " Tracker report" << std::endl;
	for(xItemItr = xObjectsActive->begin(); xItemItr != xObjectsActive->end(); ++xItemItr)
		report << "	" << xItemItr->second.mName << " active " << xItemItr->second.mCount << std::endl;
}

////////////////////////////////////////////////////////////
// Tests
#ifdef GTUT


GTUT_START(test_makehash, consistency){
	const char *quhzks = "quhzks";
	GTUT_ASRT(makeHash(quhzks) == makeHash(quhzks), "mask hash isn't consistent");
}GTUT_END;

GTUT_START(TestingFind, something){
	int MagicNum = 3;
	std::vector<int> vecA;
	std::vector<int>::iterator itrA = vecA.begin();

	vecA.push_back(1);
	vecA.push_back(2);
	vecA.push_back(MagicNum);
	vecA.push_back(4);

	GTUT_ASRT( isIn(MagicNum, vecA) == true, "Couldn't find the magic number");
}GTUT_END;



#endif

