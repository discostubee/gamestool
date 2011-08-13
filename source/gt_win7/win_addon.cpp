#include "win_addon.hpp"

using namespace gt;

void
cAddon_win::draftAddon(const dStr &pName){
	if(mLibHand == NULL){
		TCHAR exeDir[512];
		dPtrDraftAll tempPtrDraftAll = NULL;
		size_t pointless=0;

		::GetCurrentDirectory(512 - pName.length()-5, exeDir);

		::wcscat_s(exeDir, L"\\");
		::mbstowcs_s(
			&pointless, 
			&exeDir[win::MSStrLen(exeDir)], 
			pName.length()+1, 
			pName.c_str(),
			512
		);
		::wcscat_s(exeDir, L".dll");		
		
		DBUG_LO("using windows to load dynamic link library " << pName);

		mLibHand = ::LoadLibrary( exeDir );
		CHECK_WIN_ERR;

		tempPtrDraftAll = reinterpret_cast<dPtrDraftAll>(
			::GetProcAddress(mLibHand, static_cast<LPCSTR>(cAddon::xDraftAllFooStr))
		);
		CHECK_WIN_ERR;

		if(tempPtrDraftAll != NULL){
			tempPtrDraftAll(gWorld.get().get());
		}
	}
}

void
cAddon_win::closeAddon(){
	if(mLibHand != NULL){
		dPtrCloseLib tempPtrCloseLib;

		DBUG_LO("closing addon");
		
		tempPtrCloseLib = reinterpret_cast<dPtrCloseLib>(
			::GetProcAddress(mLibHand, static_cast<LPCSTR>(cAddon::xCloseAddonFooStr))
		);
		CHECK_WIN_ERR;

		if(tempPtrCloseLib != NULL){
			tempPtrCloseLib();
		}
		(void)::FreeLibrary(mLibHand);
		mLibHand = NULL;
	}
}


cAddon_win::cAddon_win():
	mLibHand(NULL)
{
}

cAddon_win::~cAddon_win(){
	closeAddon();
}
