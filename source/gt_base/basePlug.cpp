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

#include "lead.hpp"


////////////////////////////////////////////////////////////
using namespace gt;

cBase_plug::cBase_plug(){
}

cBase_plug::cBase_plug(const cBase_plug &pCopy){

}

cBase_plug::~cBase_plug(){
	try{
		for(
			dMapLeads::iterator itr = mLeadsConnected.begin();
			itr != mLeadsConnected.end();
			++itr
		){
			try{
				itr->first->unplug(this);	//- the count is irrelevant.
			}catch(excep::base_error &e){
				WARN(e);
			}catch(...){
				//- carry on removing.
			}
		}
	}catch(excep::base_error &e){
		WARN(e);
	}catch(...){
		WARN_S("Error destroying base plug.");
	}
}

void
cBase_plug::linkLead(cLead *pLead){
	PROFILE;
	ASRT_NOTNULL(pLead);

	dMapLeads::iterator itr = mLeadsConnected.find(pLead);
	if(itr==mLeadsConnected.end()){
		mLeadsConnected[pLead] = 1;
	}else{
		++itr->second;
	}
}

void
cBase_plug::unlinkLead(cLead *pLead){
	PROFILE;
	ASRT_NOTNULL(pLead);

	dMapLeads::iterator itr = mLeadsConnected.find(pLead);
	if(itr != mLeadsConnected.end()){
		--itr->second;
		if(itr->second == 0){
			mLeadsConnected.erase(itr);
		}

	}else{
		WARN_S("lead isn't connected to this plug.");
	}
}

//////////////////////////////////////////////////////////////
using namespace gt;

#ifdef GTUT

GTUT_START(test_litePlug, assign){
	int data = 3;
	tLitePlug<int> testme(&data);
	GTUT_ASRT(testme.get() == data, "lite plug didn't get assigned correctly");
}GTUT_END;

#endif
