#include "textFig.hpp"

////////////////////////////////////////////////////////////
using namespace gt;

const cPlugTag* cTextFig::xPT_text = tOutline<cTextFig>::makePlugTag("text");

const cCommand::dUID cTextFig::xSetText = tOutline<cTextFig>::makeCommand(
	"set text", &cTextFig::patSetText,
	cTextFig::xPT_text,
	NULL
);

const cCommand::dUID cTextFig::xGetText = tOutline<cTextFig>::makeCommand(
	"get text", &cTextFig::patGetText,
	cTextFig::xPT_text,
	NULL
);

cTextFig::cTextFig(){
	addUpdRoster(&mText);
}

cTextFig::~cTextFig(){
}

void
cTextFig::patSetText(ptrLead aLead){
	aLead->setPlug(&mText, xPT_text);
}

void
cTextFig::patGetText(ptrLead aLead){
	mText = aLead->getPlug(xPT_text);
}

void
cTextFig::save(cByteBuffer* pAddHere){
	mText.save(pAddHere);
}

void
cTextFig::loadEat(cByteBuffer* pBuff, dReloadMap* pReloads){
	mText.loadEat(pBuff, pReloads);
}

