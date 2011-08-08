
#include "figment.hpp"	// So we get figment.


////////////////////////////////////////////////////////////
using namespace gt;

cContext::cContext() :
	mThreadID(boost::this_thread::get_id())
{}

cContext::cContext(const cContext & copyMe) :
	mStack(copyMe.mStack),
	mTimesStacked(copyMe.mTimesStacked),
	mPlateOPancakes(copyMe.mPlateOPancakes)
{}

cContext::~cContext(){
}

void
cContext::add(dFigConSig pFig){
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
cContext::finished(dFigConSig pFig){
	PROFILE;

	ASRT_NOTNULL(pFig);

	//!\todo	Update this so that it there can be a deep nest of replaced figments.
	dNameHash pancakeHash = pFig->getReplacement() == uDoesntReplace ? pFig->hash() : pFig->getReplacement();

	//DBUG_LO("Context finishes hash " << pancakeHash);

	if(mStack.back() != pFig){
		throw excep::stackFault(mStack, "", __FUNCTION__, __LINE__);
	}

	figSigItr = mTimesStacked.find(pFig);
	if(figSigItr != mTimesStacked.end()){

		--figSigItr->second;
		if(figSigItr->second == 0){
			mTimesStacked.erase(figSigItr);
		}
	}

	cakeItr = mPlateOPancakes.find(pancakeHash);
	if(cakeItr != mPlateOPancakes.end()){

		if( cakeItr->second.back() != pFig ){
			throw excep::stackFault(mStack, "the last figment on the type stack isn't the one we expected", __FUNCTION__, __LINE__);
		}
		cakeItr->second.pop_back();

		if(cakeItr->second.empty()){
			mPlateOPancakes.erase(cakeItr);
		}
	}

	(void)mStack.pop_back();
}

bool
cContext::isStacked(dFigConSig pFig){
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
	dNameHash pancakeHash = gWorld.get()->getBlueprint(pType)->replace();

	if (pancakeHash == uDoesntReplace)
		pancakeHash = pType;

	//DBUG_LO("Context gets hash " << pancakeHash);

	cakeItr = mPlateOPancakes.find(pancakeHash);

	if(cakeItr == mPlateOPancakes.end())
		throw excep::notFound("name hash", __FILE__, __LINE__);

	return refFig(cakeItr->second.back());
}

dProgramStack
cContext::makeStackDump(){
	return mStack;
}

bool
cContext::isBlocked(){
	return false;
}

////////////////////////////////////////////////////////////
cFigContext::cFigContext() :
	currentCon(NULL)
{}

cFigContext::~cFigContext(){

}

void
cFigContext::start(cContext *con){
	PROFILE;
	boost::unique_lock<boost::mutex> lock(conMu);
	if(con->isStacked(this))
		throw excep::stackFault_selfReference(con->makeStackDump(), __FILE__, __LINE__);

	con->add(this);

	if(currentCon != NULL){
		conSync.wait(lock);
	}

	currentCon = con;
}

void
cFigContext::stop(cContext *con){
	PROFILE;
	boost::unique_lock<boost::mutex> lock(conMu);
	currentCon = NULL;
	con->finished(this);
	conSync.notify_one();
}
