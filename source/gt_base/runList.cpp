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

const cPlugTag *cRunList::xPT_single = tOutline<cRunList>::makePlugTag("single");

const cCommand::dUID cRunList::xAdd = tOutline<cRunList>::makeCommand(
	"add", &cRunList::patAdd,
	xPT_single,
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
	ASRT_NOTNULL(pCon);

	for(dList::itr_t i = mList.getItr(); i.stillGood(); ++i)
		(*i).get()->run(pCon);
}

cFigment::dMigrationPattern
cRunList::getLoadPattern(){
	PROFILE;

	dMigrationPattern pattern;
	dVersionPlugs version1;

	version1.push_back(&mList);

	pattern.push_back(version1);
	return pattern;
}

void
cRunList::getLinks(std::list<ptrFig>* pOutLinks){
	PROFILE;
	ASRT_NOTNULL(pOutLinks);
	for(dList::itr_t itr = mList.getItr(); itr.stillGood(); ++itr){
		pOutLinks->push_back(itr.get().get());
	}
}

void
cRunList::patAdd(ptrLead aLead){
	PROFILE;
	aLead->appendPlug(&mList, xPT_single);
}


#ifdef GTUT
#	include "unitTestFigments.hpp"

GTUT_START(test_cRunList, test_suit){
	tOutline<cFigment>::draft();
	figmentTestSuit<cRunList>();
}GTUT_END;

#endif

