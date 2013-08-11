#include "bitmap.hpp"

using namespace gt;

const cPlugTag *cBitmap::xPT_bitmap = tOutline<cBitmap>::makePlugTag("bitmap");

const cPlugTag *cBitmap::xPT_addSpot = tOutline<cBitmap>::makePlugTag("add spot");

const cCommand::dUID cBitmap::xSetRelation = tOutline<cBitmap>::makeCommand(
	"set relations",
	&cBitmap::patSetRelation,
	NULL
);

const cCommand::dUID cBitmap::xAdd = tOutline<cBitmap>::makeCommand(
	"add",
	&cBitmap::patAdd,
	cBitmap::xPT_bitmap,
	cBitmap::xPT_addSpot,
	NULL
);

cBitmap::cBitmap(){
}

cBitmap::~cBitmap(){
}

void
cBitmap::patSetRelation(ptrLead aLead){

}

void
cBitmap::patAdd(ptrLead aLead){
}
