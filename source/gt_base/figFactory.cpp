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

#include "figFactory.hpp"

using namespace gt;

const cPlugTag* cFigFactory::xPT_figHash = tOutline<cFigFactory>::makePlugTag("fig name");
const cPlugTag* cFigFactory::xPT_newFig = tOutline<cFigFactory>::makePlugTag("new figment");
const cPlugTag* cFigFactory::xPT_catalog = tOutline<cFigFactory>::makePlugTag("catalog");

const cCommand::dUID cFigFactory::xMakeFig = tOutline<cFigFactory>::makeCommand(
	"make figment", &cFigFactory::patMakeFig,
	cFigFactory::xPT_figHash,
	NULL
);

const cCommand::dUID cFigFactory::xGetFigCatalog = tOutline<cFigFactory>::makeCommand(
	"get catalog", &cFigFactory::patGetFigCatalog,
	NULL
);

cFigFactory::cFigFactory(){
}

cFigFactory::~cFigFactory(){
}


void cFigFactory::patMakeFig(ptrLead aLead){
	dNameHash hash;
	aLead->assignTo(&hash, xPT_figHash);

	ptrFig made = gWorld.get()->makeFig(hash);
	aLead->assignFrom(made, xPT_figHash);
}

void cFigFactory::patGetFigCatalog(ptrLead aLead){

}

////////////////////////////////////////////////////////////

#ifdef GTUT
#	include "unitTestFigments.hpp"

GTUT_START(test_figFactory, test_suit){
	figmentTestSuit<cFigFactory>();
}GTUT_END;

#endif
