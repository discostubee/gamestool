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

/*
#include "alucard.hpp"
////////////////////////////////////////////////////////////
using namespace gt;

const cPlugTag *cAlucard::xPT_figHash = tOutline<cAlucard>::makePlugTag("fig hash");
const cPlugTag *cAlucard::xPT_comUID = tOutline<cAlucard>::makePlugTag("command ID");
const cPlugTag *cAlucard::xPT_target = tOutline<cAlucard>::makePlugTag("target");
const cPlugTag *cAlucard::xPT_plug = tOutline<cAlucard>::makePlugTag("plug");
const cPlugTag *cAlucard::xPT_tag = tOutline<cAlucard>::makePlugTag("tag");

const cCommand::dUID cAlucard::xSetCommand = tOutline<cAlucard>::makeCommand(
	"set command", &cAlucard::patSetCom, xPT_comUID, NULL
);

const cCommand::dUID cAlucard::xSetTarget = tOutline<cAlucard>::makeCommand(
	"set target", &cAlucard::patSetTarget, xPT_target, NULL
);

const cCommand::dUID cAlucard::xAddPlug = tOutline<cAlucard>::makeCommand(
	"add plug", &cAlucard::patAddPlug, xPT_plug, xPT_tag, NULL
);

const cCommand::dUID cAlucard::xSetTargetConx = tOutline<cAlucard>::makeCommand(
	"set context target", &cAlucard::patAddPlugConx, xPT_figHash, NULL
);

const cCommand::dUID cAlucard::xAddPlugConx = tOutline<cAlucard>::makeCommand(
	"add context plug", &cAlucard::patAddPlugConx, xPT_figHash, xPT_tag, NULL
);


cAlucard::cAlucard():
	mCommand(cCommand::noID)
{
	addUpdRoster(&mTarget);
	addUpdRoster(&mAltTargetName);
	addUpdRoster(&mCommand);
}

cAlucard::~cAlucard(){
}


void cAlucard::getLinks(std::list<ptrFig>* pOutLinks){
}

void cAlucard::work(cContext* aCon){
	PROFILE;

}


#ifdef GTUT

#	include "unitTestFigments.hpp"

GTUT_START(test_cAlucard, test_suit){
	tOutline<cFigment>::draft();
	figmentTestSuit<cAlucard>();
}GTUT_END;

GTUT_START(test_reflection, alucardBasic){


}GTUT_END;
#endif
*/
