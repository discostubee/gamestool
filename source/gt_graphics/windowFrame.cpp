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

cWindowFrame::cWindowFrame(){
	mX = 10;
	mY = 10;
	mWidth = 100;
	mHeight = 100;
}

cWindowFrame::~cWindowFrame(){
}

void
cWindowFrame::patLink(cLead *aLead){
	mContent = aLead->getPlug(cWindowFrame::xPT_content, currentCon);
}

void
cWindowFrame::patSetCloser(cLead *aLead){
	mClosing = aLead->getPlug(cWindowFrame::xPT_closer, currentCon);
	DBUG_LO("The closer is now " << mClosing.mD->name());
}

void
cWindowFrame::patSetDim(cLead *aLead){
	try{ mX = aLead->getPlug(xPT_x, currentCon); }catch(excep::base_error){}
	try{ mY = aLead->getPlug(xPT_y, currentCon); }catch(excep::base_error){}
	try{ mWidth = aLead->getPlug(xPT_width, currentCon); }catch(excep::base_error){}
	try{ mHeight = aLead->getPlug(xPT_height, currentCon); }catch(excep::base_error){}
	refreshDim();
}


