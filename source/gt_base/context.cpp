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

#include "figment.hpp"

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
	for(dMapInfo::iterator itr = mSigInfo.begin(); itr != mSigInfo.end(); ++itr){
		itr->second.fromOtherStack = true;
	}
}

cContext::~cContext(){
	try{
		while(!mStack.empty()){
			itrInfo = mSigInfo.find(mStack.back());
			if(itrInfo != mSigInfo.end() && !itrInfo->second.fromOtherStack)
				mStack.back()->emergencyStop();
			mStack.pop_back();
		}
		gWorld.get()->unregContext(mSig);
	}catch(excep::base_error &e){
		WARN(e);
	}catch(...){
		WARN("unkown error when destroying a context.");
	}
}

void
cContext::add(cFigContext *pFig){
	PROFILE;

	ASRT_NOTNULL(pFig);

	mStack.push_back(pFig);

	itrInfo = mSigInfo.find(pFig);
	if(itrInfo == mSigInfo.end()){
		mSigInfo[pFig] = sInfo(1, pFig->hash());
	}else{
		++itrInfo->second.timesStacked;
	}
}

void
cContext::finished(cFigContext *pFig){
	PROFILE;

	ASRT_NOTNULL(pFig);

	do{
		if(mStack.empty())
			throw excep::stackFault(mStack, "context stack underflow.", __FILE__, __LINE__);

		if(mStack.back() != pFig){
			//WARN("Figment on top of the stack is correct. Forcibly unwinding until we find the right one.");	//- You'll see the warnings from the emergency stop.
			mStack.back()->emergencyStop();
			mKeepPopping = true;
		}else{
			mKeepPopping = false;
		}

		itrInfo = mSigInfo.find(mStack.back());
		if(itrInfo == mSigInfo.end())
			throw excep::stackFault(mStack, "Expected to find signature on info map", __FILE__, __LINE__);

		--itrInfo->second.timesStacked;
		if(itrInfo->second.timesStacked == 0){
			mSigInfo.erase(itrInfo);
		}

		(void)mStack.pop_back();

		if(mKeepPopping && mStack.empty())
			mKeepPopping = false;

	}while(mKeepPopping);
}

bool
cContext::isStacked(cFigContext *pFig){
	PROFILE;

	ASRT_NOTNULL(pFig);

	itrInfo = mSigInfo.find(pFig);
	if(itrInfo != mSigInfo.end())
		return true;

	return false;
}


dProgramStack
cContext::makeStackDump(){

	cWorld::lo("stack dump below:");
	for(gt::dProgramStack::reverse_iterator itr = mStack.rbegin(); itr != mStack.rend(); ++itr){
		cWorld::lo( (*itr)->name() );
	}
	return mStack;
}

ptrFig
cContext::getFirstOfType(dNameHash aType){
	for(dProgramStack::iterator itr = mStack.begin(); itr != mStack.end(); ++itr){
		if((*itr)->hash() == aType)
			return (*itr)->getSmart();
	}
	return gWorld.get()->getEmptyFig();
}

dConSig
cContext::getSig() const{
	return mSig;
}

void
cContext::addJackJob(ptrLead aLead, ptrFig aTarget){
	mJobs.push_back(sJackJob());
	mJobs.back().mLead = aLead;
	mJobs.back().mTarget = aTarget;
}

void
cContext::runJackJobs(){
	while(!mJobs.empty()){
		mJobs.front().mTarget->jack(mJobs.front().mLead, this);
		mJobs.pop_front();
	}
}

////////////////////////////////////////////////////////////
cFigContext::cFigContext() :
	currentCon(NULL)
{
	#ifdef GT_THREADS
		updating = false;
		locked = false;
	#endif

	mBlueprint = NULL;
	self  = NULL;
}

cFigContext::~cFigContext(){
	try{
		if(currentCon!=NULL)
			currentCon->finished(this);

		#ifdef GT_THREADS
			conMu.unlock();
		#endif
	}catch(excep::base_error &e){
		WARN(e);
	}catch(...){
		WARN("unknown error while destroying a figment");
	}
}

void
cFigContext::start(cContext *con){
	PROFILE;

	if(currentCon != NULL && con->isStacked(this))
		throw excep::stackFault_selfReference(con->makeStackDump(), __FILE__, __LINE__);

	#ifdef GT_THREADS
		conMu.lock();
		locked = true;
	#endif

	con->add(this);

	currentCon = con;
}

void
cFigContext::stop(cContext *con){
	PROFILE;

	if(con == currentCon){
		con->finished(this);

		if(!con->isStacked(this)){
			currentCon = NULL;
		}

		#ifdef GT_THREADS
			if(updating){
				for(itrRos = updateRoster.begin(); itrRos != updateRoster.end(); ++itrRos)
					(*itrRos)->updateFinish();
				updating = false;
			}
		#endif
	}else{
		WARN("tried to stop a figment with a context that wasn't right. Really odd");
	}

	#ifdef GT_THREADS
		locked = false;
		conMu.unlock();
	#endif
}

void
cFigContext::updatePlugs(){
	#ifdef GT_THREADS
		if(locked){
			updating = true;
			for(itrRos = updateRoster.begin(); itrRos != updateRoster.end(); ++itrRos)
				(*itrRos)->updateStart();
		}
	#endif
}

void
cFigContext::addUpdRoster(cBase_plug *pPlug){
	#ifdef GT_THREADS
		updateRoster.push_back(pPlug);
	#endif
}

void
cFigContext::remFromRoster(cBase_plug *pPlug){
	#ifdef GT_THREADS
		if(!updating){
			for(itrRos = updateRoster.begin(); itrRos != updateRoster.end(); ++itrRos){
				if(*itrRos == pPlug){
					updateRoster.erase(itrRos);
					return;
				}
			}
		}
	#endif
}

void
cFigContext::emergencyStop(){
	currentCon = NULL;
	#ifdef GT_THREADS
		if(updating){
			for(itrRos = updateRoster.begin(); itrRos != updateRoster.end(); ++itrRos)
				(*itrRos)->updateFinish();
			updating = false;
		}
		locked = false;
		conMu.unlock();
	#endif
	WARN("Emergency stop pulled");
}


//--- PLEASE NOTE ---//
//- Testing for these classes is done inside the figment source file.
