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
cLayer::jack(ptrLead pLead){
	try{
		switch(pLead->mCom->getSwitch<cLayer>()){
			case eSetDim:{
				cPlug<dUnitPix> w, h, x, y;
				try{ w = *pLead->getD(xPT_width); }catch(excep::base_error){}
				try{ h = *pLead->getD(xPT_height); }catch(excep::base_error){}
				try{ x = *pLead->getD(xPT_posX); }catch(excep::base_error){}
				try{ y = *pLead->getD(xPT_posY); }catch(excep::base_error){}

				setDim(x.mD, y.mD, w.mD, h.mD);
			}break;

			default:
				cFigment::jack(pLead);
				break;
		}
	}catch(excep::base_error &e){
		WARN(e);
	}
}

cByteBuffer&
cLayer::save(){
	cByteBuffer* saveBuff = new cByteBuffer();

	//- Must remember the order we put them in, so we can take them out in the right order.
	saveBuff->add(mRoot.save());

	return *saveBuff;
}


void
cLayer::loadEat(cByteBuffer* pBuff){
}

void
cLayer::getLinks(std::list<ptrFig>* pOutLinks){
	if(mRoot.mD->hash() != getHash<cEmptyFig>() ){
		pOutLinks->push_back(mRoot.mD);
	}
}
