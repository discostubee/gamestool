#include "win_world.hpp"

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
	//!\todo Make this suck less.
	std::string* rtnStr = new std::string(); 
	dNatChar* tempBuff = NULL;
	size_t MSStrLen = lstrlen(pString);
	size_t charsConverted = 0;
	
	if(MSStrLen > 0){
		tempBuff = new dNatChar[MSStrLen+1];
		::wcstombs_s(&charsConverted, tempBuff, MSStrLen+1, pString, MSStrLen+1);
		tempBuff[MSStrLen] = '\0';
		rtnStr->assign(tempBuff);

		delete tempBuff;
	}

	return *rtnStr;
}

dMSStr 
win::STLStrToMSStr(const dStr &pString){
	dMSStr* rtnStr = new dMSStr();

	if(!pString.empty()){
		size_t charsConverted = 0;
		TCHAR* MSStr = new TCHAR[pString.length()+1];

		mbstowcs_s(&charsConverted, MSStr, pString.size()+1, pString.data(), pString.size()+1);
		MSStr[pString.size()+1] = '\0';
		rtnStr->assign(MSStr);

		delete MSStr;
	}

	return *rtnStr;
}

size_t 
win::MSStrLen(const TCHAR* pString){
	return ::wcslen(pString);
}

HINSTANCE 
win::getInst(){
	return reinterpret_cast<gt::cWinWorld*>(gt::gWorld.get().get())->mInst;
}


////////////////////////////////////////////////////////////
using namespace gt;

dMillisec 
cWinWorld::getWinTime(){
	return 0;
}

cWinWorld::cWinWorld(HINSTANCE pInst){
	mInst = pInst;
	mProfiles->mGetTime = &getWinTime;
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