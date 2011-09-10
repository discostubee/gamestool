//testing out eclipse file linking.

#include "blueprint.hpp"

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
cBlueprint::cBlueprint():
	mHash(0),
	mReplaces(uDoesntReplace),
	mFuncMake(NULL),
	mGetCom(NULL),
	mGecPlugTag(NULL),
	mGetName(NULL)
{}

cBlueprint::~cBlueprint(){
}

ptrFig
cBlueprint::make() const{
	ASRT_NOTNULL(mFuncMake);

	return mFuncMake();
}

dNameHash
cBlueprint::hash() const{
	return mHash;
}

const char* 
cBlueprint::name() const{
	return mGetName();
}

dNameHash
cBlueprint::replace() const{
	return mReplaces;
}

const cCommand*
cBlueprint::getCom(dNameHash pHash) const{
	ASRT_NOTNULL(mGetCom);
	return mGetCom(pHash);
}

const cPlugTag* 
cBlueprint::gecPlugTag(dNameHash pPT) const{
	ASRT_NOTNULL(mGecPlugTag);
	return mGecPlugTag(pPT);
}

void
cBlueprint::addToGivenContainers(dComContainer* pComContain, dPTagContainer* pPTagContain) const{
	ASRT_NOTNULL(mComContainRef);
	ASRT_NOTNULL(mPTagContainRef);

	pComContain->insert(mComContainRef->begin(), mComContainRef->end());
	pPTagContain->insert(mPTagContainRef->begin(), mPTagContainRef->end());
}

const cBlueprint*
cBlueprint::operator = (const cBlueprint* pCopy){
	PROFILE;

	ASRT_NOTNULL(pCopy);

	if(pCopy != this)
	{
		mHash = pCopy->mHash;
		mReplaces = pCopy->mReplaces;
		mGecPlugTag = pCopy->mGecPlugTag;

		//- Unsure if these need to be freed first.
		mFuncMake = pCopy->mFuncMake;
		mGetCom = pCopy->mGetCom;
		mComContainRef = pCopy->mComContainRef;
		mPTagContainRef = pCopy->mPTagContainRef;
	}
	return this;
}

////////////////////////////////////////////////////////////

#ifdef GTUT

GTUT_START(testSelectah, setCurrent){
	cSelectah selectA;
	cSelectah::dModeID modeA = selectA.makeMode(makeHash("selectA"), "test mode A");
	cSelectah::dModeID modeB = selectA.makeMode(makeHash("selectA"), "test mode B");
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
