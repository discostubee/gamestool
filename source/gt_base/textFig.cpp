#include "textFig.hpp"

////////////////////////////////////////////////////////////
using namespace gt;

const cPlugTag* cTextFig::xPT_text = tOutline<cTextFig>::makePlugTag("text");

const cCommand* cTextFig::xSetText = tOutline<cTextFig>::makeCommand(
	"set text",
	cTextFig::eSetText,
	cTextFig::xPT_text,
	NULL
);

const cCommand* cTextFig::xGetText = tOutline<cTextFig>::makeCommand(
	"get text",
	cTextFig::eGetText,
	cTextFig::xPT_text,
	NULL
);

cTextFig::cTextFig(){
}

cTextFig::~cTextFig(){
}

void
cTextFig::run(cContext* pCon){
	start(pCon);
	DBUG_LO("text figment = " << mText.mD);
	stop(pCon);
}

void
cTextFig::jack(ptrLead pLead, cContext* pCon){
	PROFILE;

	start(pCon);
	try{
		switch( pLead->mCom->getSwitch<cTextFig>() ){
			case eSetText:
				mText = pLead->getPlug(cTextFig::xPT_text, pCon);
				break;

			case eGetText:
				pLead->setPlug( &mText, cTextFig::xPT_text, pCon );
				break;

			default:
				stop(pCon);
				cFigment::jack(pLead, pCon);
				break;
		}
	}catch(excep::base_error &e){
		WARN(e);
	}
	stop(pCon);
}

cByteBuffer&
cTextFig::save(){
	cByteBuffer* rtBuff =  new cByteBuffer();
	mText.save(rtBuff);
	return *rtBuff;
}

void
cTextFig::loadEat(cByteBuffer* pBuff, dReloadMap* pReloads){
	mText.loadEat(pBuff, pReloads);
}

