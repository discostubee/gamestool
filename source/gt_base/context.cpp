
#include "figment.hpp"	// So we get figment.

////////////////////////////////////////////////////////////
using namespace excep;

stackFault::stackFault(
	gt::dProgramStack pBadStack,
	const std::string &pMoreInfo,
	const char* pFile,
	unsigned int pLine
): base_error(pFile, pLine){
	try{
		std::stringstream ss;
		ss << "stack fault: (stack dump below) " << std::endl;
		for(gt::dProgramStack::reverse_iterator itr = pBadStack.rbegin(); itr != pBadStack.rend(); ++itr){
			ss << "   -" << (*itr)->name() << std::endl;
		}
		ss << "   More info: " << pMoreInfo;
		addInfo( ss.str() );
	}catch(...){}
}

stackFault::~stackFault() throw() {}


////////////////////////////////////////////////////////////
using namespace gt;


#ifdef GT_THREADS
cContext::cContext() :
		mThreadID(boost::this_thread::get_id())
#else
cContext::cContext()
#endif
{
	mSig = gWorld.get()->regContext(this);
}


cContext::cContext(const cContext & copyMe) :
	mStack(copyMe.mStack),
	mSigInfo(copyMe.mSigInfo)
{
	mSig = gWorld.get()->regContext(this);
}

cContext::~cContext(){
	gWorld.get()->unregContext(mSig);
}

void
cContext::add(dFigConSig pFig, dNameHash pClassID){
	PROFILE;

	ASRT_NOTNULL(pFig);

	mStack.push_back(pFig);

	itrInfo = mSigInfo.find(pFig);
	if(itrInfo == mSigInfo.end()){
		mSigInfo[pFig] = sInfo(1, pClassID);
	}else{
		++itrInfo->second.timesStacked;
	}
}

void
cContext::finished(dFigConSig pFig){
	PROFILE;

	ASRT_NOTNULL(pFig);

	if(mStack.empty())
		throw excep::stackFault(mStack, "tried to pop a figment when the stack was empty", __FILE__, __LINE__);

	if(mStack.back() != pFig)
		throw excep::stackFault(mStack, "the last figment on the type stack isn't the one we expected", __FILE__, __LINE__);

	itrInfo = mSigInfo.find(pFig);
	if(itrInfo == mSigInfo.end())
		throw excep::stackFault(mStack, "Expected to find signature on info map", __FILE__, __LINE__);

	--itrInfo->second.timesStacked;
	if(itrInfo->second.timesStacked == 0){
		mSigInfo.erase(itrInfo);
	}

	(void)mStack.pop_back();
}

bool
cContext::isStacked(dFigConSig pFig, dNameHash pID){
	PROFILE;

	ASRT_NOTNULL(pFig);

	itrInfo = mSigInfo.find(pFig);
	if(itrInfo != mSigInfo.end() && itrInfo->second.realID == pID)
		return true;

	return false;
}


dProgramStack
cContext::makeStackDump(){
	return mStack;
}

////////////////////////////////////////////////////////////
cFigContext::cFigContext() :
	currentCon(NULL)
{}

cFigContext::~cFigContext(){
	#ifdef GT_THREADS
		conMu.unlock();
	#endif
}

void
cFigContext::start(cContext *con){
	PROFILE;

	//!\todo make safe with a different context.
	if(currentCon != NULL && con->isStacked(this, hash()))
		throw excep::stackFault_selfReference(con->makeStackDump(), __FILE__, __LINE__);

	#ifdef GT_THREADS
		conMu.lock();
	#endif

	con->add(this, hash());

	currentCon = con;
}

void
cFigContext::stop(cContext *con){

	if(con == currentCon){
		PROFILE;

		//throw excep::base_error("can't stop a context that isn't yours", __FILE__, __LINE__);

		con->finished(this);

		if(!con->isStacked(this)){
			currentCon = NULL;
		}
	}

	#ifdef GT_THREADS
		conMu.unlock();
	#endif
}

void
cFigContext::kill(){
	currentCon = NULL;
	#ifdef GT_THREADS
		conMu.unlock();
	#endif
}
