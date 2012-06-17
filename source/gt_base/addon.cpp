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

#include "addon.hpp"

using namespace gt;

cAddon::dTimesOpened cAddon::xOpenAddons;

const char* cAddon::xDraftAllFooStr = "draftAll";
const char* cAddon::xCloseAddonFooStr = "closeLib";

const cPlugTag* cAddon::xPT_addonName = tOutline<cAddon>::makePlugTag("addon name");

const cCommand::dUID cAddon::xLoadAddon = tOutline<cAddon>::makeCommand(
	"load addon",
	&cAddon::patLoadAddon,
	cAddon::xPT_addonName,
	NULL
);

cAddon::cAddon():
	mAddonHash(0)
{
	addUpdRoster(&mAddonName);
}

cAddon::~cAddon(){
	try{
		dTimesOpened::iterator found = xOpenAddons.find(mAddonHash);
		if(found != xOpenAddons.end()){
			--found->second;
			if(found->second <= 0){
				closeAddon();
				xOpenAddons.erase(found);
			}
		}
	}catch(...){
	}
}

void
cAddon::run(cContext* pCon){
	start(pCon);
	updatePlugs();

	if(mAddonHash == 0){
		mAddonHash = makeHash(
			toNStr(
				mAddonName.get()
			)
		);

		dTimesOpened::iterator found = xOpenAddons.find(mAddonHash);
		if(found != xOpenAddons.end()){
			++found->second;
		}else{
			draftAddon(mAddonName.get());
			++xOpenAddons[mAddonHash];
		}
	}
	stop(pCon);
}

iFigment::dMigrationPattern
cAddon::getLoadPattern(){
	dMigrationPattern mp;
	dVersionPlugs vp;
	vp.push_back(mAddonName);
	mp.push_back(vp);
	return mp;
}

void
cAddon::patLoadAddon(ptrLead aLead){
	if(mAddonHash == 0){
		mAddonName = aLead->getPlug(xPT_addonName);
	}
}

#ifdef GTUT

GTUT_START(test_cAddon, test_suit){
	tOutline<cFigment>::draft();
	figmentTestSuit<cAddon>();
}GTUT_END;

#endif

