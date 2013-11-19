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
*/

#include "utils.hpp"

////////////////////////////////////////////////////////////
dHash makeHash(const char *pString){
	dNameHash hash = 0;

	if(pString==NULL)
		return 0;

	while(*pString!='\0'){
		hash = ((hash << 5) + hash) ^ *pString;
		++pString;
	}

	return hash;
}

dNameHash makeHash(const dNatStr &pString){
	dNameHash hash = 0;

	for(size_t i=0; i < pString.t.size(); ++i)
		hash = ((hash << 5) + hash) ^ pString.t[i];

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
			objectsActiveSetup = false;
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
	xItemItr = xObjectsActive->find(xTempHash);
	if(xItemItr != xObjectsActive->end())
		--xItemItr->second.mCount;
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

	vecA.push_back(1);
	vecA.push_back(2);
	vecA.push_back(MagicNum);
	vecA.push_back(4);

	GTUT_ASRT( isIn(MagicNum, vecA) == true, "Couldn't find the magic number");
}GTUT_END;



#endif

