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

#include "selectah.hpp"

using namespace gt;

////////////////////////////////////////////////////////////
cSelectah::mapStrID cSelectah::xSHLookup;
std::vector<cSelectah::sMode> cSelectah::xAllModes;

cSelectah::cSelectah(){
	currentMode = myModes.end();
}

cSelectah::~cSelectah(){
}

cSelectah::dModeID
cSelectah::makeMode(dNameHash figOwner, const dStr &aName){
	dNameHash hash = makeHash(aName.c_str());
	xAllModes.push_back(sMode());
	sMode* ptrMode = &xAllModes.back();
	ptrMode->name = aName;
	ptrMode->ID = xAllModes.size();
	xSHLookup[hash] = ptrMode->ID;
	myModes[ptrMode->ID] = ptrMode;

	if(currentMode == myModes.end())
		currentMode = myModes.find(ptrMode->ID);

	return ptrMode->ID;
}

void
cSelectah::set(dModeID mode){
	tmpItr = myModes.find(mode);
	if(tmpItr == myModes.end())
		throw excep::notFound("mode", __FILE__, __LINE__);

	currentMode = tmpItr;
}

cSelectah::dModeID
cSelectah::current(){
	return currentMode->second->ID;
}

bool
cSelectah::has(dModeID mode){
	tmpItr = myModes.find(mode);
	if(tmpItr != myModes.end())
		return true;

	return false;
}

cSelectah::sMode&
cSelectah::getModeFromAll(const dStr &aName){
	mapStrID::iterator itr = xSHLookup.find(makeHash(aName.c_str()));
	if(itr == xSHLookup.end())
		throw excep::notFound("mode", __FILE__, __LINE__);

	return xAllModes[itr->second];
}

void
cSelectah::cleanupAll(){
}


////////////////////////////////////////////////////////////

#ifdef GTUT

GTUT_START(testSelectah, setCurrent){
	cSelectah selectA;
	cSelectah::dModeID modeA = selectA.makeMode(makeHash("selectA"), "test mode A");
	cSelectah::dModeID modeB = selectA.makeMode(makeHash("selectA"), "test mode B");
	selectA.set(modeA);
	GTUT_ASRT(selectA.current()==modeA, "mode not set.");
	selectA.set(modeB);
	GTUT_ASRT(selectA.current()==modeB, "mode not set.");
}GTUT_END;

GTUT_START(testSelectah, preventCrossModes){
	bool caught = false;
	cSelectah selectA;
	cSelectah selectB;
	cSelectah::dModeID modeA = selectA.makeMode(makeHash("selectA"), "test mode A");
	cSelectah::dModeID modeB = selectB.makeMode(makeHash("selectB"), "test mode B");
	try{
		selectA.set(modeB);
		selectB.set(modeA);
	}catch(excep::notFound){
		caught = true;
	}
	GTUT_ASRT(caught, "didn't catch cross mode set.");
}GTUT_END;

GTUT_START(testSelectah, getFromAll){
}GTUT_END;

#endif
