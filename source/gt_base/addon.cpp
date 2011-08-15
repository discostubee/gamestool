#include "addon.hpp"

using namespace gt;

cAddon::dTimesOpened cAddon::xOpenAddons;

const char* cAddon::xDraftAllFooStr = "draftAll";
const char* cAddon::xCloseAddonFooStr = "closeLib";

const cPlugTag* cAddon::xPT_addonName = tOutline<cAddon>::makePlugTag("addon name");

const cCommand* cAddon::xLoadAddon = tOutline<cAddon>::makeCommand(
	"load addon",
	cAddon::eLoadAddon,
	cAddon::xPT_addonName,
	NULL
);

cAddon::cAddon()
{}

cAddon::~cAddon(){
	try{
		dTimesOpened::iterator found = xOpenAddons.find(mAddonHash);
		if(found != xOpenAddons.end()){
			--found->second;
			if(found->second <= 0)
				closeAddon();
		}
	}catch(...){
	}
}

void
cAddon::jack(ptrLead pLead, cContext* pCon){
	PROFILE;

	start(pCon);
	try{
		switch( pLead->mCom->getSwitch<cAddon>() ){

			case eLoadAddon:{
				if(mAddonName.mD.empty()){
					dTimesOpened::iterator found;

					mAddonName = pLead->getPlug(cAddon::xPT_addonName, pCon)->getCopy<dStr>();
					mAddonHash = makeHash(mAddonName.mD.c_str());
					found = xOpenAddons.find(mAddonHash);
					if(found != xOpenAddons.end()){
						++found->second;
					}else{
						++xOpenAddons[mAddonHash];
						draftAddon(mAddonName.mD);
					}
				}
			}break;

			default:
				stop(pCon);
				return cFigment::jack(pLead, pCon);
				break;
		}
	}catch(excep::base_error &e){
		WARN(e);
	}
	stop(pCon);
}


