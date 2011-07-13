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
cFigment::jack(ptrLead pLead){
	PROFILE;

	try{
		switch( pLead->mCom->getSwitch<cFigment>() ){
			case eSave:{
				cPlug<cByteBuffer>* saveBuff = new cPlug<cByteBuffer>;
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
	gWorld->mKeepLooping = false;
}

////////////////////////////////////////////////////////////

#ifdef GTUT

GTUT_START(figment, polymorphNames){
	tOutline<cFigment>::draft();
	tOutline<cEmptyFig>::draft();
	tOutline<cWorldShutoff>::draft();

	ptrFig stdFig = gWorld->makeFig(getHash<cFigment>());
	ptrFig emptyFig = gWorld->makeFig(getHash<cEmptyFig>());
	ptrFig shutoff = gWorld->makeFig(getHash<cWorldShutoff>());

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

	ptrLead save = gWorld->makeLead(getHash<cFigment>(), cFigment::xSave->mID);
	ptrFig testMe = gWorld->makeFig(getHash<cFigment>());

	testMe->jack(save);
	cBase_plug *saveBuff = save->getD(cFigment::xPT_buffer);
	saveBuff->getMDPtr<cByteBuffer>();

}GTUT_END;

#endif
