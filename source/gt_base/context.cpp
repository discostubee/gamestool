
#include "figment.hpp"	// So we get figment.


////////////////////////////////////////////////////////////
using namespace gt;

cContext::cContext(){
}

cContext::cContext(const cContext & copyMe){

}

cContext::~cContext(){
}

void
cContext::add(iFigment* pFig){
	PROFILE;

	ASRT_NOTNULL(pFig);

	dNameHash pancakeHash = pFig->getReplacement() == uDoesntReplace ? pFig->hash() : pFig->getReplacement();

	//DBUG_LO("Context adds "<< pFig->name());

	mStack.push_back(pFig);

	figSigItr = mTimesStacked.find(pFig);
	if(figSigItr == mTimesStacked.end()){
		mTimesStacked[pFig] = 1;
	}else{
		++figSigItr->second;
	}

	cakeItr = mPlateOPancakes.find(pancakeHash);
	if(cakeItr == mPlateOPancakes.end()){
		mPlateOPancakes[pancakeHash].push_back(pFig);
	}else{
		cakeItr->second.push_back(pFig);
	}
}

void
cContext::finished(iFigment* pFig){
	PROFILE;

	ASRT_NOTNULL(pFig);

	//!\todo	Update this so that it there can be a deep nest of replaced figments.
	dNameHash pancakeHash = pFig->getReplacement() == uDoesntReplace ? pFig->hash() : pFig->getReplacement();

	//DBUG_LO("Context finishes hash " << pancakeHash);

	if(mStack.back() != pFig){
		throw excep::stackFault(mStack, "", __FUNCTION__, __LINE__);
	}

	figSigItr = mTimesStacked.find(pFig);
	if(figSigItr == mTimesStacked.end()){
		throw excep::stackFault(mStack, "", __FUNCTION__, __LINE__);
	}
	--figSigItr->second;
	if(figSigItr->second == 0){
		mTimesStacked.erase(figSigItr);
	}

	cakeItr = mPlateOPancakes.find(pancakeHash);
	if(cakeItr == mPlateOPancakes.end()){
		throw excep::stackFault(mStack, "", __FUNCTION__, __LINE__);
	}
	{
		if( cakeItr->second.back() != pFig ){
			throw excep::stackFault(mStack, "", __FUNCTION__, __LINE__);
		}
		cakeItr->second.pop_back();

		if(cakeItr->second.empty()){
			mPlateOPancakes.erase(cakeItr);
		}
	}

	(void)mStack.pop_back();
}

bool
cContext::isStacked(iFigment* pFig){
	PROFILE;

	ASRT_NOTNULL(pFig);

	figSigItr = mTimesStacked.find(pFig);
	if(figSigItr != mTimesStacked.end())
		return true;

	return false;
}

refFig
cContext::getLastOfType(dNameHash pType){
	PROFILE;

	// We need to get the base type.
	//!\todo make this deal with deep nests of blueprints.
	dNameHash pancakeHash = gWorld->getBlueprint(pType)->replace();

	if (pancakeHash == uDoesntReplace)
		pancakeHash = pType;

	//DBUG_LO("Context gets hash " << pancakeHash);

	cakeItr = mPlateOPancakes.find(pancakeHash);

	if(cakeItr == mPlateOPancakes.end())
		throw excep::notFound("name hash", __FILE__, __LINE__);

	return refFig(cakeItr->second.back());
}
