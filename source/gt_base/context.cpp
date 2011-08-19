
#include "figment.hpp"	// So we get figment.


////////////////////////////////////////////////////////////
using namespace gt;


#ifdef GT_THREADS
cContext::cContext() :
		mThreadID(boost::this_thread::get_id())
{}
#else
cContext::cContext()
{}
#endif


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

	if(mStack.empty() || mStack.back() != pFig)
		throw excep::stackFault(mStack, "", __FUNCTION__, __LINE__);

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
	currentCon(NULL),
	alreadyPopped(false)
{}

cFigContext::~cFigContext(){

}

void
cFigContext::start(cContext *con){
	PROFILE;
	#ifdef GT_THREADS
		boost::unique_lock<boost::mutex> lock(conMu);
	#endif
	if(con->isStacked(this))
		throw excep::stackFault_selfReference(con->makeStackDump(), __FILE__, __LINE__);

	con->add(this);

	#ifdef GT_THREADS
		if(currentCon != NULL){
			conSync.wait(lock);
		}
	#endif

	currentCon = con;
}

void
cFigContext::stop(cContext *con, bool nestedStop){
	PROFILE;
	#ifdef GT_THREADS
		boost::unique_lock<boost::mutex> lock(conMu);
	#endif
	if(nestedStop && alreadyPopped)
		throw excep::base_error( "Can't pop twice", __FUNCTION__, __LINE__);

	if(alreadyPopped)
		return;

	if(con != currentCon)
		throw excep::base_error("can't stop a context that isn't yours", __FUNCTION__, __LINE__);

	if(!alreadyPopped)
		con->finished(this);

	if(nestedStop)
		alreadyPopped = true;

	currentCon = NULL;
	#ifdef GT_THREADS
		conSync.notify_one();
	#endif
}
