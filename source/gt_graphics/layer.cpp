#include "layer.hpp"

using namespace gt;

const cPlugTag* cLayer::xPT_content = tOutline<cLayer>::makePlugTag("content");
const cPlugTag*	cLayer::xPT_size = tOutline<cLayer>::makePlugTag("size");
const cPlugTag*	cLayer::xPT_point = tOutline<cLayer>::makePlugTag("point");
const cPlugTag*	cLayer::xPT_rectangle = tOutline<cLayer>::makePlugTag("rectangle");
const cPlugTag*	cLayer::xPT_arrangement = tOutline<cLayer>::makePlugTag("arrangement");
const cPlugTag*	cLayer::xPT_cropStyle = tOutline<cLayer>::makePlugTag("cropStyle");

const cCommand* cLayer::xLinkContent = tOutline<cLayer>::makeCommand(
	"link content",
	cLayer::eLinkContent,
	xPT_content,
	NULL
);

const cCommand*	cLayer::xSetSize = tOutline<cLayer>::makeCommand(
	"set size",
	cLayer::eSetSize,
	xPT_size,
	NULL
);

const cCommand*	cLayer::xSetPos = tOutline<cLayer>::makeCommand(
	"set pos",
	cLayer::eSetPos,
	xPT_point,
	xPT_arrangement,
	NULL
);

const cCommand*	cLayer::xSetCrop = tOutline<cLayer>::makeCommand(
	"set crop",
	cLayer::eSetCrop,
	xPT_rectangle,
	xPT_cropStyle,
	NULL
);

cLayer::cLayer(){
}

cLayer::~cLayer(){
}

void
cLayer::jack(ptrLead pLead, cContext* pCon){
	start(pCon);
	try{
		switch(pLead->mCom->getSwitch<cLayer>()){

			case eLinkContent:{
				mLink = pLead->getPlug(xPT_content, pCon);
			}break;

			default:
				cFigment::jack(pLead, pCon);
				break;
		}
	}catch(excep::base_error &e){
		WARN(e);
	}
	stop(pCon);
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
