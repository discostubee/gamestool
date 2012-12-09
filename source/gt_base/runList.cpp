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

#include "runList.hpp"

////////////////////////////////////////////////////////////
using namespace gt;

const cCommand::dUID cRunList::xAdd = tOutline<cRunList>::makeCommand(
	"add", &cRunList::patAdd,
	NULL
);

cRunList::cRunList(){
	addUpdRoster(&mList);
}

cRunList::~cRunList(){
}

void
cRunList::work(cContext* pCon){
	PROFILE;

	for(dList::itr_t i = mList.mContainer.begin(); i != mList.mContainer.end(); ++i){
		(*i).get()->run(pCon);
	}
}

cFigment::dMigrationPattern
cRunList::getLoadPattern(){
	dMigrationPattern pattern;
	dVersionPlugs version1;

	version1.push_back(mList);

	pattern.push_back(version1);
	return pattern;
}

void
cRunList::getLinks(std::list<ptrFig>* pOutLinks){
	ASRT_NOTNULL(pOutLinks);

	for(dList::itr_t i = mList.mContainer.begin(); i != mList.mContainer.end(); ++i){
		pOutLinks->push_back( i->get() );
	}
}

void
cRunList::patAdd(ptrLead aLead){
	PROFILE;

	std::vector< tPlug<ptrFig> > plugs;
	aLead->getPile(&plugs);

	for(dList::itr_t itr = plugs.begin(); itr != plugs.end(); ++itr){
		mList.mContainer.push_back( *itr );
	}
}

////////////////////////////////////////////////////////////
using namespace gt;

const cPlugTag* cValves::xPT_state = tOutline<cValves>::makePlugTag("valve state");
const cPlugTag* cValves::xPT_valveIdx = tOutline<cValves>::makePlugTag("Valve index");

const cCommand::dUID cValves::xSetState = tOutline<cValves>::makeCommand(
	"set valve",
	&cValves::patSetValve,
	NULL
);

cValves::cValves(){
}

cValves::~cValves(){
}

void
cValves::work(cContext* pCon){
	PROFILE;

	for(size_t idx = 0; idx != mList.mContainer.size(); ++idx){
		if( mStates.mContainer[idx].get() )
			mList.mContainer[idx].get()->run(pCon);
	}
}

cFigment::dMigrationPattern
cValves::getLoadPattern(){
	dMigrationPattern pattern;
	dVersionPlugs version1;

	version1.push_back(mList);
	version1.push_back(mStates);

	pattern.push_back(version1);
	return pattern;
}

void
cValves::patSetValve(ptrLead pLead){
	//!\todo
}

#ifdef GTUT

GTUT_START(test_cRunList, test_suit){
	tOutline<cFigment>::draft();
	figmentTestSuit<cRunList>();
}GTUT_END;

GTUT_START(test_cValves, test_suit){
	tOutline<cFigment>::draft();
	tOutline<cRunList>::draft();
	figmentTestSuit<cValves>();
}GTUT_END;

#endif

