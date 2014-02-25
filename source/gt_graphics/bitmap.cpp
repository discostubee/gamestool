#include "bitmap.hpp"

using namespace gt;

const cPlugTag *cBitmap::xPT_pic = tOutline<cBitmap>::makePlugTag("pic");

const cPlugTag *cBitmap::xPT_loc = tOutline<cBitmap>::makePlugTag("loc");

const cPlugTag *cBitmap::xPT_fig = tOutline<cBitmap>::makePlugTag("figment");

const cCommand::dUID cBitmap::xAdd = tOutline<cBitmap>::makeCommand(
	"add",
	&cBitmap::patAdd,
	cBitmap::xPT_pic,
	cBitmap::xPT_loc,
	NULL
);

const cCommand::dUID cBitmap::xLinkRebuilder = tOutline<cBitmap>::makeCommand(
	"add",
	&cBitmap::patLinkRebuilder,
	cBitmap::xPT_fig,
	NULL
);

cBitmap::cBitmap(){
}

cBitmap::~cBitmap(){
}

void
cBitmap::patAdd(ptrLead aLead){
}

void
cBitmap::patLinkRebuilder(ptrLead aLead){

}
