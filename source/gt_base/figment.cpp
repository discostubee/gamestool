#include "figment.hpp"


////////////////////////////////////////////////////////////
using namespace gt;

const cPlugTag* cFigment::xPT_saveData = tOutline<cFigment>::makePlugTag("save data");

const cCommand::dUID cFigment::xSave = tOutline<cFigment>::makeCommand(
	"save", &cFigment::patSave, cFigment::xPT_saveData,
	NULL
);

const cCommand::dUID cFigment::xLoad = tOutline<cFigment>::makeCommand(
	"load", &cFigment::patLoad, cFigment::xPT_saveData,
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
cFigment::patSave(cLead *aLead){
	cByteBuffer buff;
	save( &buff );
}

void
cFigment::patLoad(cLead *aLead){
	cByteBuffer buff;
	loadEat( &buff );
}

void
cFigment::jack(ptrLead pLead, cContext* pCon){
	PROFILE;

	if(pLead->mConx != pCon->mSig)
		throw excep::badContext(__FILE__, __LINE__);

	start(pCon);
	try{
		cLead::dLockLead lock(pLead->muLead);
		ASRT_NOTNULL(mBlueprint);
		mBlueprint->getCom(pLead->mCom)->use(this, pLead.get());
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

GTUT_START(test_figment, polymorphNames){
	tOutline<cFigment>::draft();
	tOutline<cEmptyFig>::draft();
	tOutline<cWorldShutoff>::draft();

	ptrFig stdFig = gWorld.get()->makeFig(getHash<cFigment>());
	ptrFig emptyFig = gWorld.get()->makeFig(getHash<cEmptyFig>());
	ptrFig shutoff = gWorld.get()->makeFig(getHash<cWorldShutoff>());

	GTUT_ASRT(
		strncmp(stdFig->name(), cFigment::identify(), 20)==0,
		"figment names don't match."
	);

	GTUT_ASRT(
		strncmp(emptyFig->name(), cEmptyFig::identify(), 20)==0,
		"Empty figment names don't match."
	);

	GTUT_ASRT(
		strncmp(shutoff->name(), cWorldShutoff::identify(), 20)==0,
		"World shutoff names don't match."
	);

}GTUT_END;

GTUT_START(test_figment, hashes){
	tOutline<cFigment>::draft();
	cFigment test;

	GTUT_ASRT(getHash<cFigment>()==test.hash(), "hashes don't match");
}GTUT_END;




//!\brief	Really basic class for testing out the context.
class testContextFigment: public cFigment{
public:
	testContextFigment *refOther;

	testContextFigment() : refOther(NULL) {}
	virtual ~testContextFigment() {}

	static const char* identify(){ return "test context figment"; }
	virtual const char* name() const { return identify(); }
	virtual dNameHash hash() const { return getHash<testContextFigment>(); }

	virtual void run(cContext *pCon){
		start(pCon);
		if(refOther) refOther->run(pCon);
		stop(pCon);
	}
};

GTUT_START(test_context, preventSelfReference){
	cContext testMe;
	bool caughtLikeABoss = false;
	testContextFigment A, B;

	A.refOther = &B;
	B.refOther = &A;
	try{ A.run(&testMe); }catch(excep::stackFault_selfReference){ caughtLikeABoss = true; }
	GTUT_ASRT(caughtLikeABoss, "context: Not the boss");
}GTUT_END;

#endif
