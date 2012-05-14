#include "figment.hpp"


////////////////////////////////////////////////////////////
using namespace gt;

const cPlugTag* cFigment::xPT_serialBuff = tOutline<cFigment>::makePlugTag("serial buffer");
const cPlugTag* cFigment::xPT_loadingParty = tOutline<cFigment>::makePlugTag("loading party");

const cCommand::dUID cFigment::xSave = tOutline<cFigment>::makeCommand(
	"save", &cFigment::patSave, cFigment::xPT_serialBuff,
	NULL
);

const cCommand::dUID cFigment::xLoad = tOutline<cFigment>::makeCommand(
	"load", &cFigment::patLoad, cFigment::xPT_serialBuff,
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
cFigment::save(cByteBuffer* pSaveHere){
	dMigrationPattern loadPattern = getLoadPattern();

	if(loadPattern.empty())
		return;
}

void
cFigment::loadEat(cByteBuffer* pLoadFrom, dReloadMap *aReloads){
	dMigrationPattern loadPattern = getLoadPattern();

	if(loadPattern.empty())
		return;

	tPlug<dNumVer> numVer;

	numVer.loadEat(pLoadFrom);

	if(numVer.mD > loadPattern.size())
		throw excep::fromTheFuture(__FILE__, __LINE__);

	std::vector<dPlugHolder>::iterator itrPrev, itrPrevEnd, itrCur;

	for(size_t idxVer = numVer.mD; idxVer < loadPattern.size(); ++idxVer){
		itrCur = loadPattern[idxVer].begin();
		if(idxVer==0){
			while(itrCur != loadPattern[0].end()){
				itrPrev->get().loadEat( pLoadFrom, aReloads );
				++itrCur;
			}

		}else{
			itrPrev = loadPattern[idxVer-1].begin();
			itrPrevEnd = loadPattern[idxVer-1].end();
			for(itrCur=loadPattern[idxVer].begin(); itrCur!=loadPattern[idxVer].end(); ++itrCur){
				if(itrPrev != itrPrevEnd){
					itrCur->get() = itrPrev->get();
					++itrPrev;

				}else{
					break;
				}
			}
		}
	}
}

void
cFigment::patSave(ptrLead aLead){
	tPlug<ptrBuff> buffer = aLead->getPlug(xPT_serialBuff);

	save(buffer.mD.get());
}

void
cFigment::patLoad(ptrLead aLead){
	tPlug<ptrBuff> buffer = aLead->getPlug(xPT_serialBuff);

	loadEat(buffer.mD.get());
}

void
cFigment::jack(ptrLead pLead, cContext* pCon){
	PROFILE;

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

cFigment::dMigrationPattern
cFigment::getLoadPattern(){
	return dMigrationPattern();
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

GTUT_START(test_bufferPlug, allGood){

}GTUT_END;

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

class testMigration_v1: public cFigment{
private:
public:
	tPlug<int> mCat;

	testMigration_v1(){}
	virtual ~testMigration_v1(){}

	static const dNatChar* identify(){ return "test migration figment"; }
	virtual const dNatChar* name() const { return identify(); }
	virtual dNameHash hash() const { return getHash<testMigration_v1>(); }
	static dNumVer version(){ return 1; }
	virtual dNumVer getVersion() const { return version(); }

};

class testMigration_v2: public cFigment{
private:
public:
	tPlug<int> mCat, mMoo;

	testMigration_v2(){}
	virtual ~testMigration_v2(){}

	static const dNatChar* identify(){ return "test migration figment"; }
	virtual const dNatChar* name() const { return identify(); }
	virtual dNameHash hash() const { return getHash<testMigration_v1>(); }
	static dNumVer version(){ return 2; }
	virtual dNumVer getVersion() const { return version(); }

protected:

};

GTUT_START(test_figment, migration){

}GTUT_END;


//!\brief	Really basic class for testing out the context.
class testContextFigment: public cFigment{
public:
	testContextFigment *refOther;
	bool throwOnRun;

	testContextFigment() : refOther(NULL), throwOnRun(false) {}
	virtual ~testContextFigment() {}

	static const dNatChar* identify(){ return "test context figment"; }
	virtual const dNatChar* name() const { return identify(); }
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
	cWorld::suppressNextError();
	try{ A.run(&testMe); }catch(excep::base_error){}
	A.stop(&testMe);
	GTUT_ASRT( !B.stillStacked(), "B is still stacked." );
}GTUT_END;

#endif
