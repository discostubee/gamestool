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

#include "win_world.hpp"
#include "gt_base/figment.hpp"

////////////////////////////////////////////////////////////
using namespace win;

void
win::checkError(const char* pFile, unsigned int pLine){
	DWORD errCode = ::GetLastError();

	::SetLastError(0);
	if(errCode != 0){
		TCHAR* tempStr;
		excep::base_error winError(pFile, pLine);

		::FormatMessage(
			FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM |FORMAT_MESSAGE_IGNORE_INSERTS,
			NULL,
			errCode,
			MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
			(LPTSTR)&tempStr,
			0,
			NULL
		);

		winError.addInfo(MSStrToSTLStr(tempStr));

		if(tempStr != NULL)
			::LocalFree(tempStr);

		throw winError;
	}
}

dStr 
win::MSStrToSTLStr(const TCHAR* pString){
	
#ifdef UNICODE
	//!\todo Make this suck less.
	std::string rtnStr; 

	dNatChar* tempBuff = NULL;
	size_t MSStrLen = lstrlen(pString);
	size_t charsConverted = 0;
	
	if(MSStrLen > 0){
		tempBuff = new dNatChar[MSStrLen+1];
		::wcstombs_s(
			&charsConverted, 
			tempBuff, 
			MSStrLen+1, 
			pString, 
			MSStrLen+1
		);
		tempBuff[MSStrLen] = '\0';
		rtnStr->assign(tempBuff);

		delete tempBuff;
	}
	return rtnStr;

#else
	return dStr(pString);
#endif
}

dMSStr 
win::STLStrToMSStr(const dStr &pString){
#ifdef UNICODE
	dMSStr rtnStr;

	if(!pString.empty()){
		size_t charsConverted = 0;
		TCHAR* MSStr = new TCHAR[pString.length()+1];

		::mbstowcs_s(
			&charsConverted, 
			MSStr, 
			pString.size()+1, 
			pString.data(), 
			pString.size()+1
		);
		MSStr[pString.size()+1] = '\0';
		rtnStr->assign(MSStr);

		delete MSStr;
	}

	return rtnStr;
#else
	return dMSStr(pString.c_str());
#endif
}

size_t 
win::MSStrLen(const TCHAR* pString){
#ifdef UNICODE
	return ::wcslen(pString);
#else
	return strlen(pString);
#endif
}


////////////////////////////////////////////////////////////
using namespace gt;

::HINSTANCE	cWinWorld::xInst;

void cWinWorld::setWinInst(::HINSTANCE aInst){
	xInst = aInst;
}

::HINSTANCE cWinWorld::getWinInst(){
	return xInst;
}

dMillisec 
cWinWorld::getWinTime(){
	return 0;
}

cWinWorld::cWinWorld(){
	mProfiler->mGetTime = &getWinTime;
}

cWinWorld::~cWinWorld() {
	flushLines();
}

dMillisec
cWinWorld::getAppTime(){
	return getWinTime();
}

void 
cWinWorld::loop() {
	cContext worldContext;

	DBUG_LO("windows world loop");
	while(mKeepLooping){
		mRoot->run(&worldContext);
		flushLines();
	}

	flushLines();
}

void 
cWinWorld::flushLines(){
	for(dLines::iterator i = mLines->begin(); i != mLines->end(); ++i){
		std::cout << (*i) << std::endl;
	}
	mLines->clear();
}

void		
cWinWorld::copyWorld(cWorld* pWorld){
	cWorld::copyWorld(pWorld);
}