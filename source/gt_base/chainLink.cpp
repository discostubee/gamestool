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

#include "chainLink.hpp"

////////////////////////////////////////////////////////////
using namespace gt;

const cCommand::dUID cChainLink::xSetLink = tOutline<cChainLink>::makeCommand(
	"set link", &cChainLink::patSetLink,
	xPT_links,
	NULL
);

cChainLink::cChainLink(){
	addUpdRoster(&mLink);
}

cChainLink::~cChainLink(){

}

void
cChainLink::work(cContext* pCon){
	if(mLink.get().valid())
		mLink.get()->run(pCon);
}

cFigment::dMigrationPattern
cChainLink::getLoadPattern(){
	PROFILE;

	dMigrationPattern pattern;
	dVersionPlugs version1;

	version1.push_back(&mLink);

	pattern.push_back(version1);
	return pattern;
}

void
cChainLink::getLinks(std::list<ptrFig>* pOutLinks){
	if(mLink.get().valid())
		pOutLinks->push_back(mLink.get());
}

void
cChainLink::patSetLink(ptrLead aLead){
	preLink();
	if(!aLead->copyPlug(&mLink, xPT_links))
		WARN_S("Unable to copy plug");
	postLink();
}

void
cChainLink::preLink(){
	//- Base version does nothing.
}

void
cChainLink::postLink(){
	//- Base version does nothing.
}


#ifdef GTUT
#include "unitTestFigments.hpp"
GTUT_START(test_chainLink, test_suit){
	figmentTestSuit<cChainLink>();
}GTUT_END;

#endif

