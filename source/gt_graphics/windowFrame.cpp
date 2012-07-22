#include "windowFrame.hpp"

using namespace gt;

const cPlugTag* cWindowFrame::xPT_content = tOutline<cWindowFrame>::makePlugTag("content");
const cPlugTag* cWindowFrame::xPT_closer = tOutline<cWindowFrame>::makePlugTag("closer");
const cPlugTag* cWindowFrame::xPT_x = tOutline<cWindowFrame>::makePlugTag("x pos");
const cPlugTag* cWindowFrame::xPT_y = tOutline<cWindowFrame>::makePlugTag("y pos");
const cPlugTag* cWindowFrame::xPT_width = tOutline<cWindowFrame>::makePlugTag("width");
const cPlugTag* cWindowFrame::xPT_height = tOutline<cWindowFrame>::makePlugTag("height");

const cCommand::dUID cWindowFrame::xLinkContent = tOutline<cWindowFrame>::makeCommand(
	"link content", &cWindowFrame::patLink,
	cWindowFrame::xPT_content,
	NULL
);

const cCommand::dUID cWindowFrame::xSetCloser = tOutline<cWindowFrame>::makeCommand(
	"link closer", &cWindowFrame::patSetCloser,
	cWindowFrame::xPT_closer,
	NULL
);

const cCommand::dUID cWindowFrame::xSetDim = tOutline<cWindowFrame>::makeCommand(
	"set dimensions", &cWindowFrame::patSetDim,
	cWindowFrame::xPT_x,
	cWindowFrame::xPT_y,
	cWindowFrame::xPT_height,
	cWindowFrame::xPT_width,
	NULL
);

cWindowFrame::cWindowFrame() :
	mX(10), mY(10), mWidth(100), mHeight(100)
{
	addUpdRoster(&mContent);
	addUpdRoster(&mClosing);
	addUpdRoster(&mX);
	addUpdRoster(&mY);
	addUpdRoster(&mWidth);
	addUpdRoster(&mHeight);
}

cWindowFrame::~cWindowFrame(){
}

void
cWindowFrame::patLink(ptrLead aLead){
	mContent = aLead->getPlug(cWindowFrame::xPT_content);
}

void
cWindowFrame::patSetCloser(ptrLead aLead){
	mClosing = aLead->getPlug(cWindowFrame::xPT_closer);
	DBUG_LO("The closer is now " << mClosing.get()->name());
}

void
cWindowFrame::patSetDim(ptrLead aLead){
	aLead->setPlug(&mX, xPT_x, true);
	aLead->setPlug(&mY, xPT_y, true);
	aLead->setPlug(&mWidth, xPT_width, true);
	aLead->setPlug(&mHeight, xPT_height, true);
	refreshDim();
}


