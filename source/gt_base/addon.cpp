#include "addon.hpp"

using namespace gt;

std::vector<cAddon::ptrStr> cAddon::xOpenAddons;

const char* cAddon::xDraftAllFooStr = "draftAll";
const char* cAddon::xCloseAddonFooStr = "closeLib";

const cPlugTag* cAddon::xPT_addonName = tOutline<cAddon>::makePlugTag("addon name");

const cCommand* cAddon::xLoadAddon = tOutline<cAddon>::makeCommand(
	"load addon",
	cAddon::eLoadAddon,
	cAddon::xPT_addonName,
	NULL
);

cAddon::cAddon():
	mAddonName(ptrStr(new dStr))
{
}

cAddon::~cAddon(){
	try{
		for(
			std::vector<ptrStr>::iterator itr = xOpenAddons.begin();
			itr != xOpenAddons.end();
			++itr
		){
			if(itr->use_count()==1){
				std::vector<ptrStr>::iterator afterDel = itr;
				++afterDel;
				xOpenAddons.erase(itr);
				if(afterDel == xOpenAddons.end()){
					break;
				}else{
					itr = afterDel;
				}
			}
		}
	}catch(...){
	}
}

void
cAddon::requirements(){
	//tOutline<cFigment>::draft();
}

void
cAddon::jack(ptrLead pLead){
	PROFILE;
	try{
		switch( pLead->mCom->getSwitch<cAddon>() ){

			case eLoadAddon:{
				if(mAddonName->empty()){
					std::vector<ptrStr>::iterator itr;

					*mAddonName = *pLead->getD(cAddon::xPT_addonName)->getMDPtr<dStr>();

					for(
						itr = xOpenAddons.begin();
						itr != xOpenAddons.end();
						++itr
					){
						if( mAddonName->compare(*itr->get())==0 ){
							mAddonName = *itr;
						}
					}

					if(itr == xOpenAddons.end()){
						draftAddon(*mAddonName);
						xOpenAddons.push_back(mAddonName);
					}else{
						DBUG_LO("That library is already open.");
					}
				}
			}break;

			default:
				return cFigment::jack(pLead);

				break;
		}
	}catch(excep::base_error &e){
		WARN(e);
	}
}

