#include "layer.hpp"

using namespace gt;

const cPlugTag* cLayer::xPT_content = tOutline<cLayer>::makePlugTag("content");
const cPlugTag*	cLayer::xPT_size = tOutline<cLayer>::makePlugTag("size");
const cPlugTag*	cLayer::xPT_point = tOutline<cLayer>::makePlugTag("point");
const cPlugTag*	cLayer::xPT_rectangle = tOutline<cLayer>::makePlugTag("rectangle");
const cPlugTag*	cLayer::xPT_arrangement = tOutline<cLayer>::makePlugTag("arrangement");
const cPlugTag*	cLayer::xPT_cropStyle = tOutline<cLayer>::makePlugTag("cropStyle");

const cCommand::dUID cLayer::xLinkContent = tOutline<cLayer>::makeCommand(
	"link content", &cLayer::patSetLink,
	xPT_content,
	NULL
);

const cCommand::dUID cLayer::xSetLayout = tOutline<cLayer>::makeCommand(
	"set layout", &cLayer::patSetLayout,
	xPT_size,
	xPT_point,
	xPT_rectangle,
	xPT_arrangement,
	xPT_cropStyle,
	NULL
);

const cCommand::dUID cLayer::xGetLayout = tOutline<cLayer>::makeCommand(
	"getLayout", &cLayer::patGetLayout,
	xPT_size,
	xPT_point,
	xPT_rectangle,
	xPT_arrangement,
	xPT_cropStyle,
	NULL
);

cLayer::cLayer(){
}

cLayer::~cLayer(){
}

void
cLayer::save(cByteBuffer* pAddHere){
	tPlug< sWH<dUnitVDis> > size;
	tPlug< sPoint2D<dUnitVDis> > pos;
	tPlug< shape::rectangle<dUnitPix32> > crop;

	size = getSize();
	pos = getPos();
	crop = getCrop();

	//- The order of these saves must be the same in load.
	mLink.save(pAddHere);
	size.save(pAddHere);
	pos.save(pAddHere);
	crop.save(pAddHere);
}

void
cLayer::loadEat(cByteBuffer* pBuff, dReloadMap* pReloads){
	tPlug< sWH<dUnitVDis> > size;
	tPlug< sPoint2D<dUnitVDis> > pos;
	tPlug< shape::rectangle<dUnitPix32> > crop;

	mLink.loadEat(pBuff, pReloads);
	size.loadEat(pBuff, pReloads);
	pos.loadEat(pBuff, pReloads);
	crop.loadEat(pBuff, pReloads);

	setSize(size.mD);
	setPos(pos.mD);
	setCrop(crop.mD);
}

void
cLayer::getLinks(std::list<ptrFig>* pOutLinks){
	if(mLink.mD->hash() != getHash<cEmptyFig>() )
		pOutLinks->push_back(mLink.mD);
}

void
cLayer::patSetLink(cLead *aLead){
	mLink = aLead->getPlug(xPT_content, currentCon);
}

void
cLayer::patSetLayout(cLead *aLead){

}

void
cLayer::patGetLayout(cLead *aLead){

}
