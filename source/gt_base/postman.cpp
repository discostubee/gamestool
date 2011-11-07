#include "postman.hpp"

////////////////////////////////////////////////////////////
using namespace gt;


const cPlugTag *cPostman::xPT_target = tOutline<cPostman>::makePlugTag("target");
const cPlugTag *cPostman::xPT_command = tOutline<cPostman>::makePlugTag("command");

//const cCommand::dUID cPostman::xSetupPostman = tOutline<cPostman>::makeCommand(
//		"setup", &cPostman::patSetup, cPostman::xPT_target, cPostman::xPT_command, NULL
//);

void
cPostman::run(cContext* pCon){

}

void
cPostman::patSetup(cLead *aLead){

}
