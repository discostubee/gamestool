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

#include "command.hpp"
#include "figment.hpp"

////////////////////////////////////////////////////////////
using namespace gt;

const cCommand::dUID cCommand::noID = static_cast<cCommand::dUID>(-1);

cCommand::cCommand(
	const dUID pID,
	const char* pName,
	const dNameHash pParentHash
):
	mID(pID), mName(pName), mParent(pParentHash)
{
	DBUG_TRACK_START("command");
}

cCommand::~cCommand(){
	DBUG_TRACK_END("command");
}

////////////////////////////////////////////////////////////
// Tests
#ifdef GTUT

class cComTestFig: public cFigment, private tOutline<cComTestFig>{
public:
	static const cPlugTag* xPT_testTag;

	static const dNatChar* identify(){ return "command test figment"; }

	cComTestFig(){}
	~cComTestFig(){}
};

const cPlugTag* cComTestFig::xPT_testTag = tOutline<cComTestFig>::makePlugTag("test tag");

GTUT_START(command, nothing){

}
GTUT_END;

#endif
