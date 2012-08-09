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
	mCopyIdx = ORIGINAL;
	mSig = gWorld.get()->regContext(this);
}


cContext::cContext(const cContext & copyMe) :
	mStack(copyMe.mStack),
	mStackInfo(copyMe.mStackInfo),
	mCopyIdx(copyMe.mStack.size()-1)
{
	mSig = gWorld.get()->regContext(this);
}

cContext::~cContext(){
	try{
		while( static_cast<int>(mStack.size()) > (mCopyIdx + 1)){
			itrInfo = mStackInfo.find(mStack.back());
			if(itrInfo == mStackInfo.end())
				throw excep::stackFault(mStack, "Expected to find signature on info map", __FILE__, __LINE__);

			--itrInfo->second.timesStacked;
			if(itrInfo->second.timesStacked == 0){
				mStackInfo.erase(itrInfo);
			}

			mStack.back()->emergencyStop();
			mStack.pop_back();
		}
		gWorld.get()->unregContext(mSig);
	}catch(excep::base_error &e){
		WARN(e);
	}catch(...){
		WARN_S("unkown error when destroying a context.");
	}
}

void
cContext::add(cFigContext *pFig){
	PROFILE;

	ASRT_NOTNULL(pFig);

	mStack.push_back(pFig);

	itrInfo = mStackInfo.find(pFig);
	if(itrInfo == mStackInfo.end()){
		mStackInfo[pFig] = sInfo(1, pFig->hash());
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
			//WARN_S("Figment on top of the stack is incorrect. Forcibly unwinding until we find the right one.");	//- You'll see the warnings from the emergency stop.
			mStack.back()->emergencyStop();
			mKeepPopping = true;
		}else{
			mKeepPopping = false;
		}

		itrInfo = mStackInfo.find(mStack.back());
		if(itrInfo == mStackInfo.end())
			throw excep::stackFault(mStack, "Expected to find signature on info map", __FILE__, __LINE__);

		--itrInfo->second.timesStacked;
		if(itrInfo->second.timesStacked == 0){
			mStackInfo.erase(itrInfo);
		}

		(void)mStack.pop_back();

		if(mStack.empty())
			mKeepPopping = false;

		if( mCopyIdx < static_cast<int>(mStack.size()) )
			mKeepPopping = false;

	}while(mKeepPopping);
}

bool
cContext::isStacked(cFigContext *pFig){
	PROFILE;

	ASRT_NOTNULL(pFig);

	itrInfo = mStackInfo.find(pFig);
	return (itrInfo != mStackInfo.end());
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
	PROFILE;

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

cContext&
cContext::operator=(const cContext &pCon){
	ASRT_NOTSELF(&pCon);

	mStack = pCon.mStack;
	mStackInfo = pCon.mStackInfo;
	mCopyIdx = pCon.mCopyIdx;

	return *this;
}

////////////////////////////////////////////////////////////
cFigContext::cFigContext() :
	currentCon(NULL),
	locked(false),
	flushMe(NULL)
{
	//- From parent.
	mBlueprint = NULL;
	self = NULL;

	//-
	bufferLeads = &ALeads;

#	ifdef GT_THREADS
		updating = false;
#	endif
}

cFigContext::~cFigContext(){
	try{
		if(currentCon!=NULL)
			currentCon->finished(this);

#		ifdef GT_THREADS
			if(locked)
				muCon.unlock();
#		endif
	}catch(excep::base_error &e){
		WARN(e);
	}catch(...){
		WARN_S("unknown error while destroying a figment");
	}
}

void
cFigContext::start(cContext *con){
	PROFILE;

	if(currentCon != NULL && con->isStacked(this))	//- prevent deadlocks.
		throw excep::stackFault_selfReference(con->makeStackDump(), __FILE__, __LINE__);

#	ifdef GT_THREADS
		muCon.lock();
#	endif

	locked = true;
	flushMe = NULL;

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

#		ifdef GT_THREADS
			if(updating){
				for(itrRos = updateRoster.begin(); itrRos != updateRoster.end(); ++itrRos)
					(*itrRos)->updateFinish();
				updating = false;
			}
#		endif
	}else{
		WARN_S("tried to stop a figment with a context that wasn't right. Really odd");
	}

#	ifdef GT_THREADS
		locked = false;
		muCon.unlock();
#	endif
}

void
cFigContext::addUpdRoster(cBase_plug *pPlug){
#	ifdef GT_THREADS
		updateRoster.push_back(pPlug);
#	endif
}

void
cFigContext::updatePlugs(){
	PROFILE;
#	ifdef GT_THREADS
		if(locked){
			updating = true;
			for(itrRos = updateRoster.begin(); itrRos != updateRoster.end(); ++itrRos)
				(*itrRos)->updateStart();
		}
#	endif
}

void
cFigContext::remFromRoster(cBase_plug *pPlug){
	PROFILE;
#	ifdef GT_THREADS
		if(!updating){
			for(itrRos = updateRoster.begin(); itrRos != updateRoster.end(); ++itrRos){
				if(*itrRos == pPlug){
					updateRoster.erase(itrRos);
					return;
				}
			}
		}
#	endif
}



void
cFigContext::emergencyStop(){
	PROFILE;
	currentCon = NULL;

	if(locked){
#		ifdef GT_THREADS
			if(updating){
				for(itrRos = updateRoster.begin(); itrRos != updateRoster.end(); ++itrRos)
					(*itrRos)->updateFinish();
				updating = false;
			}
			locked = false;
			muCon.unlock();
#		else
			locked = false;
#		endif
	}

	WARN_S("Emergency stop pulled for" << name());
}

bool
cFigContext::differedLead(ptrLead pLead){
	PROFILE;

	if(locked){
#		ifdef GT_THREADS
			dLock lockList(muLeads);
#		endif

		bufferLeads->push_back(pLead);
		return true;
	}
	return false;
}

ptrLead
cFigContext::processLeads(){
	PROFILE;
	ptrLead rtnLead;

	if(flushMe == NULL){
#		ifdef GT_THREADS
			dLock lockList(muLeads);
#		endif

		flushMe = bufferLeads;

		if(bufferLeads == &ALeads)
			bufferLeads = &BLeads;
		else
			bufferLeads = &ALeads;
	}

	if(flushMe->empty()){
		flushMe = NULL;

	}else{
		rtnLead = flushMe->front();
		flushMe->pop_front();
	}
	return rtnLead;

}

//--- PLEASE NOTE ---//
//- Testing for these classes is done inside the figment source file.
