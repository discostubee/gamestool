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

#include "valve.hpp"

////////////////////////////////////////////////////////////
using namespace gt;

const cPlugTag* cValve::xPT_link = tOutline<cValve>::makePlugTag("link");
const cPlugTag* cValve::xPT_state = tOutline<cValve>::makePlugTag("state");

const cCommand::dUID cValve::xSetLink = tOutline<cValve>::makeCommand(
	"set link", &cValve::patSetLink,
	cValve::xPT_link,
	NULL
);

const cCommand::dUID cValve::xSetState = tOutline<cValve>::makeCommand(
	"set state", &cValve::patSetState,
	cValve::xPT_state,
	NULL
);

const cCommand::dUID cValve::xGetState = tOutline<cValve>::makeCommand(
	"get state", &cValve::patGetState,
	cValve::xPT_state,
	NULL
);

cValve::cValve(){
	addUpdRoster(&mLink);
	addUpdRoster(&mState);

	mState = true;
}

cValve::~cValve(){
}

void
cValve::getLinks(std::list<ptrFig>* pOutLinks){
	pOutLinks->push_back(mLink.get());
}

void
cValve::patSetLink(ptrLead aLead){
	aLead->setPlug(&mLink, xPT_link);
}

void
cValve::patSetState(ptrLead aLead){
	aLead->setPlug(&mState, xPT_state);
}

void
cValve::patGetState(ptrLead aLead){
	aLead->copyPlug(&mState, xPT_state);
}

iFigment::dMigrationPattern
cValve::getLoadPattern(){
	dMigrationPattern pattern;
	dVersionPlugs version1;

	version1.push_back(&mLink);
	version1.push_back(&mState);

	pattern.push_back(version1);
	return pattern;
}


////////////////////////////////////////////////////////////

#ifdef GTUT
#include "unitTestFigments.hpp"

GTUT_START(test_cValve, test_suit){
	tOutline<cValve>::draft();
	figmentTestSuit<cValve>();
}GTUT_END;

#endif

