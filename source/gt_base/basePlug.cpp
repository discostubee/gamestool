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

#include "basePlug.hpp"

using namespace gt;

////////////////////////////////////////////////////////////
cBase_plug::cBase_plug(dPlugType pTI):
	mType(pTI)
{
}

cBase_plug::cBase_plug(const cBase_plug &pCopy):
	mType(pCopy.mType)
{
}

cBase_plug::~cBase_plug(){
}

void
cBase_plug::linkLead(cLead *pLead){
	PROFILE;
	ASRT_NOTNULL(pLead);

	itrLead = mLeadsConnected.find(pLead);
	if(itrLead==mLeadsConnected.end()){
		mLeadsConnected[pLead] = 1;
	}else{
		++itrLead->second;
	}
}

void
cBase_plug::unlinkLead(cLead *pLead){
	PROFILE;
	ASRT_NOTNULL(pLead);

	itrLead = mLeadsConnected.find(pLead);
	if(itrLead != mLeadsConnected.end()){
		--itrLead->second;
		if(itrLead->second == 0){
			mLeadsConnected.erase(itrLead);
		}

	}else{
		WARN("lead isn't connected to this plug.");
	}
}

size_t
cBase_plug::numLeadsConnected(){
	return mLeadsConnected.size();
}

