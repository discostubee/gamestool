#include "figment.hpp"


////////////////////////////////////////////////////////////
using namespace gt;

const cPlugTag* cFigment::xPT_buffer = tOutline<cFigment>::makePlugTag("buffer");

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
			case eSave:
				save( pLead->getPlug(cFigment::xPT_buffer, pCon)->getPtr<cByteBuffer>() );
			break;

			case eLoad:
				loadEat( pLead->getPlug(cFigment::xPT_buffer, pCon)->getPtr<cByteBuffer>() );
			break;

			default:
			case eNotMyBag:
				DBUG_LO("not my bag");
			break;
		}
	}catch(excep::base_error &e){
		WARN(e);
	}catch(...){
		UNKNOWN_ERROR;
	}
	stop(pCon);
}


void 
cFigment::run(cContext* pCon){
	DUMB_REF_ARG(pCon);
}

void
cFigment::save(cByteBuffer* pAddHere){
	DUMB_REF_ARG(pAddHere);
}

void 
cFigment::loadEat(cByteBuffer* pBuff, dReloadMap* pReloads){
	DUMB_REF_ARG(pBuff); DUMB_REF_ARG(pReloads);
}

void 
cFigment::getLinks(std::list<ptrFig>* pOutLinks){
	DUMB_REF_ARG(pOutLinks);
}

////////////////////////////////////////////////////////////
using namespace gt;

cEmptyFig::cEmptyFig(){
	//DBUG_LO("empty fig made @ " << reinterpret_cast<unsigned long>(this));
}

cEmptyFig::~cEmptyFig(){
	//DBUG_LO("empty fig destroyed @ " << reinterpret_cast<unsigned long>(this));
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
	ptrLead save = gWorld.get()->makeLead(getHash<cFigment>(), cFigment::xSave->mID, &fake);
	ptrFig testMe = gWorld.get()->makeFig(getHash<cFigment>());

	tPlug<cByteBuffer> saveBuff;
	save->add(&saveBuff, cFigment::xPT_buffer, &fake);
	testMe->jack(save, &fake);


}GTUT_END;

#endif
