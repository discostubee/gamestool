#include "windowFrame.hpp"

using namespace gt;

const cPlugTag* cWindowFrame::xPT_content = tOutline<cWindowFrame>::makePlugTag("content");
const cPlugTag* cWindowFrame::xPT_closer = tOutline<cWindowFrame>::makePlugTag("closer");
const cPlugTag* cWindowFrame::xPT_x = tOutline<cWindowFrame>::makePlugTag("x pos");
const cPlugTag* cWindowFrame::xPT_y = tOutline<cWindowFrame>::makePlugTag("y pos");
const cPlugTag* cWindowFrame::xPT_width = tOutline<cWindowFrame>::makePlugTag("width");
const cPlugTag* cWindowFrame::xPT_height = tOutline<cWindowFrame>::makePlugTag("height");

const cCommand* cWindowFrame::xLinkContent = tOutline<cWindowFrame>::makeCommand(
	"link content",
	cWindowFrame::eLinkContent,
	cWindowFrame::xPT_content,
	NULL
);

const cCommand* cWindowFrame::xSetCloser = tOutline<cWindowFrame>::makeCommand(
	"link closer",
	cWindowFrame::eLinkCloseFig,
	cWindowFrame::xPT_closer,
	NULL
);

const cCommand* cWindowFrame::xSetDim = tOutline<cWindowFrame>::makeCommand(
	"set dimensions",
	cWindowFrame::eChangeDim,
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
cWindowFrame::jack(ptrLead pLead, cContext *pCon){
	PROFILE;

	start(pCon);
	try{
		switch( pLead->mCom->getSwitch<cWindowFrame>() ){
			case eLinkContent:
				mContent = *pLead->getPlug(cWindowFrame::xPT_content, pCon);
				break;
			case eLinkCloseFig:
				mClosing = *pLead->getPlug(cWindowFrame::xPT_closer, pCon);
				DBUG_LO("The closer is now " << mClosing.mD->name());
				break;
			case eChangeDim:{
					try{ mX = *pLead->getPlug(xPT_x, pCon); }catch(excep::base_error){}
					try{ mY = *pLead->getPlug(xPT_y, pCon); }catch(excep::base_error){}
					try{ mWidth = *pLead->getPlug(xPT_width, pCon); }catch(excep::base_error){}
					try{ mHeight = *pLead->getPlug(xPT_height, pCon); }catch(excep::base_error){}
					
					refreshDim();
				}break;
			default:
				cFigment::jack(pLead, pCon);
				break;
		}
	}catch(excep::base_error &e){
		WARN(e);
	}catch(...){
		UNKNOWN_ERROR;
	}
	stop(pCon);
}
