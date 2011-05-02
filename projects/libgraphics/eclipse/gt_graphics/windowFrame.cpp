#include "windowFrame.hpp"

using namespace gt;

const cPlugTag* cWindowFrame::xPT_content = tOutline<cWindowFrame>::makePlugTag("addon name");
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
	mWidth = 256;
	mHeight = 256;
}

cWindowFrame::~cWindowFrame(){
}

void 
cWindowFrame::jack(ptrLead pLead){
	PROFILE;

	try{
		switch( pLead->mCom->getSwitch<cWindowFrame>() ){
			case eLinkContent:
				mContent = *pLead->getD(cWindowFrame::xPT_content);
				break;
			case eLinkCloseFig:
				mClosing = *pLead->getD(cWindowFrame::xPT_closer);
				DBUG_LO("The closer is now " << mClosing.mD->name());
				break;
			case eChangeDim:{
					try{ mX = *pLead->getD(xPT_x); }catch(excep::base_error){}
					try{ mY = *pLead->getD(xPT_y); }catch(excep::base_error){}
					try{ mWidth = *pLead->getD(xPT_width); }catch(excep::base_error){}
					try{ mHeight = *pLead->getD(xPT_height); }catch(excep::base_error){}
					
					refreshDim();
				}break;
			default:
				cFigment::jack(pLead);
				break;
		}
	}catch(excep::base_error &e){
		WARN(e);
	}catch(...){
		UNKNOWN_ERROR;
	}
}
