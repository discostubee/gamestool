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

#include "postman.hpp"

////////////////////////////////////////////////////////////
using namespace gt;


const cPlugTag *cPostman::xPT_target = tOutline<cPostman>::makePlugTag("target");
const cPlugTag *cPostman::xPT_command = tOutline<cPostman>::makePlugTag("command");

//const cCommand::dUID cPostman::xSetupPostman = tOutline<cPostman>::makeCommand(
//		"setup", &cPostman::patSetup, cPostman::xPT_target, cPostman::xPT_command, NULL
//);

cPostman::cPostman(){
	addUpdRoster(&mTarget);
}

cPostman::~cPostman(){

}

void
cPostman::run(cContext* pCon){
	start(pCon);
	updatePlugs();
	stop(pCon);
}

void
cPostman::patSetup(cLead *aLead){

}
