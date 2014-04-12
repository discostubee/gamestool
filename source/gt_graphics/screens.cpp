#include "screens.hpp"

using namespace gt;

const cPlugTag*	cScreen::xPT_rect = tOutline<cScreen>::makePlugTag("rectangle");
const cPlugTag*	cScreen::xPT_ID = tOutline<cScreen>::makePlugTag("ID");

const cCommand::dUID cScreen::xGetInfo = tOutline<cScreen>::makeCommand(
	"get info",
	&cScreen::patGetInfo,
	cScreen::xPT_rect,
	cScreen::xPT_ID,
	NULL
);

const cCommand::dUID cScreen::xSetID = tOutline<cScreen>::makeCommand(
	"set ID",
	&cScreen::patSetID,
	cScreen::xPT_ID,
	NULL
);


void cScreen::patGetInfo(ptrLead pLead){
	pLead->linkPlug(&mID, xPT_ID);
	tPlug< shape::tRectangle<dUnitPix> > tmp( getDims() );
	(void)pLead->setPlug(&tmp, xPT_rect);
}

void cScreen::patSetID(ptrLead pLead){
	(void)pLead->copyPlug(&mID, xPT_ID);
}

cScreen::cScreen(){
	addUpdRoster(&mID);
}

cScreen::~cScreen(){
}

cFigment::dMigrationPattern
cScreen::getLoadPattern(){
	PROFILE;

	dMigrationPattern pattern;
	dVersionPlugs version1;

	version1.push_back(&mID);

	pattern.push_back(version1);
	return pattern;
}


