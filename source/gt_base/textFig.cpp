#include "textFig.hpp"

////////////////////////////////////////////////////////////
using namespace gt;

const tPlugTag* cTextFig::xPT_text = tOutline<cTextFig>::makePlugTag("text");

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
	CON_START(pCon);
	DBUG_LO("text figment = " << mText.mD);
	CON_STOP(pCon);
}

void
cTextFig::jack(ptrLead pLead, cContext* pCon){
	try{
		switch( pLead->mCom->getSwitch<cTextFig>() ){
			case eSetText:
				mText = *pLead->getD(cTextFig::xPT_text);
				break;

			case eGetText:
				pLead->add( &mText, cTextFig::xPT_text );
				break;

			default:
				cFigment::jack(pLead, pCon);
				break;
		}
	}catch(excep::base_error &e){
		WARN(e);
	}
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

