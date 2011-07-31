#include "figment.hpp"


////////////////////////////////////////////////////////////
using namespace gt;

const tPlugTag* cFigment::xPT_buffer = tOutline<cFigment>::makePlugTag("buffer");

const cCommand* cFigment::xSave = tOutline<cFigment>::makeCommand(
	"save",
	cFigment::eSave,
	cFigment::xPT_buffer,
	NULL
);

const cCommand* cFigment::xLoad = tOutline<cFigment>::makeCommand(
	"load",
	cFigment::eLoad,
	cFigment::xPT_buffer,
	NULL
);

cFigment::cFigment(){
	//DBUG_LO("...making figment " << reinterpret_cast<unsigned int>(this))
	DBUG_TRACK_START("figment");
}

cFigment::~cFigment(){
	//DBUG_LO("...destroying figment " << reinterpret_cast<unsigned int>(this))
	DBUG_TRACK_END("figment");
}

void
cFigment::jack(ptrLead pLead, cContext* pCon){
	PROFILE;

	start(pCon);
	try{
		switch( pLead->mCom->getSwitch<cFigment>() ){
			case eSave:{
				tPlug<cByteBuffer>* saveBuff = new tPlug<cByteBuffer>;
				save(&saveBuff->mD);
				pLead->take(saveBuff, cFigment::xPT_buffer );
			}break;

			case eLoad:{
				loadEat( pLead->getD(cFigment::xPT_buffer)->getMDPtr<cByteBuffer>() );
			}break;

			default:
			case eNotMyBag:{
				DBUG_LO("not my bag");
			}break;
		}
	}catch(excep::base_error &e){
		WARN(e);
	}catch(...){
		UNKNOWN_ERROR;
	}
	stop(pCon);
}


void
cFigment::save(cByteBuffer* pAddHere){
}

////////////////////////////////////////////////////////////
using namespace gt;

cEmptyFig::cEmptyFig(){
	DBUG_LO("empty fig made @ " << reinterpret_cast<unsigned long>(this));
}

cEmptyFig::~cEmptyFig(){
	DBUG_LO("empty fig destroyed @ " << reinterpret_cast<unsigned long>(this));
}

////////////////////////////////////////////////////////////
using namespace gt;

cWorldShutoff::cWorldShutoff(){
}

cWorldShutoff::~cWorldShutoff(){
}

void
cWorldShutoff::run(cContext* pCon){
	start(pCon);
	gWorld.get()->mKeepLooping = false;
	stop(pCon);

}

////////////////////////////////////////////////////////////

#ifdef GTUT

GTUT_START(figment, polymorphNames){
	tOutline<cFigment>::draft();
	tOutline<cEmptyFig>::draft();
	tOutline<cWorldShutoff>::draft();

	ptrFig stdFig = gWorld.get()->makeFig(getHash<cFigment>());
	ptrFig emptyFig = gWorld.get()->makeFig(getHash<cEmptyFig>());
	ptrFig shutoff = gWorld.get()->makeFig(getHash<cWorldShutoff>());

	GTUT_ASRT(
		strncmp(stdFig->name(), cFigment::identify(), 10)==0,
		"figment names don't match."
	);

	GTUT_ASRT(
		strncmp(emptyFig->name(), cEmptyFig::identify(), 10)==0,
		"Empty figment names don't match."
	);

	GTUT_ASRT(
		strncmp(shutoff->name(), cWorldShutoff::identify(), 10)==0,
		"World shutoff names don't match."
	);

}GTUT_END;

GTUT_START(figment, hashes){
	tOutline<cFigment>::draft();
	cFigment test;

	GTUT_ASRT(getHash<cFigment>()==test.hash(), "hashes don't match");
}GTUT_END;

GTUT_START(figment, givesSave){
	tOutline<cFigment>::draft();

	cContext fake;
	ptrLead save = gWorld.get()->makeLead(getHash<cFigment>(), cFigment::xSave->mID);
	ptrFig testMe = gWorld.get()->makeFig(getHash<cFigment>());

	testMe->jack(save, &fake);
	cBase_plug *saveBuff = save->getD(cFigment::xPT_buffer);
	saveBuff->getMDPtr<cByteBuffer>();

}GTUT_END;

#endif
