#include "layer.hpp"

using namespace gt;

const cPlugTag* cLayer::xPT_root = tOutline<cLayer>::makePlugTag("root");
const cPlugTag* cLayer::xPT_width = tOutline<cLayer>::makePlugTag("width");
const cPlugTag* cLayer::xPT_height = tOutline<cLayer>::makePlugTag("height");
const cPlugTag* cLayer::xPT_posX = tOutline<cLayer>::makePlugTag("position X");
const cPlugTag* cLayer::xPT_posY = tOutline<cLayer>::makePlugTag("position Y");

const cCommand* cLayer::xSetDim = tOutline<cLayer>::makeCommand(
	"set dimensions",
	cLayer::eSetDim,
	xPT_width,
	xPT_height,
	xPT_posX,
	xPT_posY,
	NULL
);

const cCommand* cLayer::xSetRoot = tOutline<cLayer>::makeCommand(
	"set root",
	cLayer::eSetRoot,
	xPT_root,
	NULL
);

cLayer::cLayer(){
}

cLayer::~cLayer(){
}

void
cLayer::jack(ptrLead pLead, cContext* pCon){
	try{
		switch(pLead->mCom->getSwitch<cLayer>()){
			case eSetDim:{
				tPlug<dUnitPix> w, h, x, y;
				try{ w = *pLead->getPlug(xPT_width, pCon); }catch(excep::base_error){}
				try{ h = *pLead->getPlug(xPT_height, pCon); }catch(excep::base_error){}
				try{ x = *pLead->getPlug(xPT_posX, pCon); }catch(excep::base_error){}
				try{ y = *pLead->getPlug(xPT_posY, pCon); }catch(excep::base_error){}

				setDim(x.mD, y.mD, w.mD, h.mD);
			}break;

			default:
				cFigment::jack(pLead, pCon);
				break;
		}
	}catch(excep::base_error &e){
		WARN(e);
	}
}

void
cLayer::save(cByteBuffer* pAddHere){
	mRoot.save(pAddHere);
}

void
cLayer::loadEat(cByteBuffer* pBuff, dReloadMap* pReloads){
	mRoot.loadEat(pBuff, pReloads);
}

void
cLayer::getLinks(std::list<ptrFig>* pOutLinks){
	if(mRoot.mD->hash() != getHash<cEmptyFig>() ){
		pOutLinks->push_back(mRoot.mD);
	}
}
