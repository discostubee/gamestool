#include "win_addon.hpp"

using namespace gt;

void
cAddon_win::draftAddon(const dStr &pName){
	if(mLibHand == NULL){
		TCHAR addonPath[512];
		dPtrDraftAll tempPtrDraftAll = NULL;
		size_t pointless=0;

		CHECK_WIN_ERR;

		::GetCurrentDirectory(512 - pName.length()-5, addonPath);

		::wcscat_s(addonPath, L"\\addon");
		::mbstowcs_s(
			&pointless, 
			&addonPath[win::MSStrLen(addonPath)], 
			pName.length()+1, 
			pName.c_str(),
			512
		);

#if defined(DEBUG) && defined(GT_THREADS)
		::wcscat_s(addonPath, L"_dt.dll");
#elif defined(DEBUG)
		::wcscat_s(addonPath, L"_d.dll");
#else
		::wcscat_s(addonPath, L".dll");
#endif

		DBUG_LO("using windows to load dynamic link library " << pName);
		
		mLibHand = ::LoadLibrary( addonPath );
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
