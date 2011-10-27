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

#if defined(DEBUG) && defined(GT_SPEED)
	const cPlugTag* cFigment::xPT_life = tOutline<cFigment>::makePlugTag("life");
	const cCommand::dUID cFigment::xTestJack = tOutline<cFigment>::makeCommand(
		"test jack", &cFigment::patTestJack, cFigment::xPT_life,
		NULL
	);

#endif

cFigment::cFigment(){
	DBUG_TRACK_START("figment");
}

cFigment::~cFigment(){
	DBUG_TRACK_END("figment");
}

void
cFigment::patSave(ptrLead aLead){
	cByteBuffer buff;
	save( &buff );
}

void
cFigment::patLoad(ptrLead aLead){
	cByteBuffer buff;
	loadEat( &buff );
}

void
cFigment::jack(ptrLead pLead, cContext* pCon){
	PROFILE;

	//- May not need this anymore.
	//if(pLead->mConx != pCon->mSig)
	//	throw excep::badContext(__FILE__, __LINE__);

	start(pCon);
	try{
		#ifdef GT_THREADS
			cLead::dLockLead lock(pLead->muLead);
		#endif
		ASRT_NOTNULL(mBlueprint);
		mBlueprint->getCom(pLead->mCom)->use(this, pLead);
	}catch(excep::base_error &e){
		std::stringstream ss;
		ss << name() << e.what();
		WARN(ss.str().c_str());
	}catch(...){
		UNKNOWN_ERROR;
	}
	stop(pCon);
}


void 
cFigment::run(cContext* pCon){
	DUMB_REF_ARG(pCon);
	//start(pCon);
	//stop(pCon);
}

void
cFigment::save(cByteBuffer* pAddHere){
	DUMB_REF_ARG(pAddHere);
	//start(pCon);
	//stop(pCon);
}

void 
cFigment::loadEat(cByteBuffer* pBuff, dReloadMap *aReloads){
	DUMB_REF_ARG(pBuff); DUMB_REF_ARG(aReloads);
}

void 
cFigment::getLinks(std::list<ptrFig>* pOutLinks){
	DUMB_REF_ARG(pOutLinks);
	//start(pCon);
	//stop(pCon);
}

#if defined(DEBUG) && defined(GT_SPEED)
	void cFigment::patTestJack(ptrLead aLead){
		tPlug<int> gotIt;
		gotIt = aLead->getPlug(xPT_life);
		if(gotIt.mD != 42)
			throw std::exception();
	}
#endif

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
	bool throwOnRun;

	testContextFigment() : refOther(NULL), throwOnRun(false) {}
	virtual ~testContextFigment() {}

	static const char* identify(){ return "test context figment"; }
	virtual const char* name() const { return identify(); }
	virtual dNameHash hash() const { return getHash<testContextFigment>(); }

	virtual void run(cContext *pCon){
		start(pCon);
		if(throwOnRun)
			throw excep::base_error(__FILE__, __LINE__);
		else
			if(refOther) refOther->run(pCon);
		stop(pCon);
	}

	bool stillStacked(){ return currentCon != NULL; }
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

GTUT_START(test_context, forceUnwind){
	cContext testMe;
	testContextFigment A, B;
	A.refOther = &B;
	B.throwOnRun = true;
	try{ A.run(&testMe); }catch(excep::base_error){}
	A.stop(&testMe);
	GTUT_ASRT( !B.stillStacked(), "B is still stacked." );
}GTUT_END;

#endif
