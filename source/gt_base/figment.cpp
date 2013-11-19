/*
 **********************************************************************************************************
 *  Copyright (C) 2010  Stuart Bridgens
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License (version 3) as published by
 *  the Free Software Foundation.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *********************************************************************************************************
*/

#include "figment.hpp"
#include "plugContainer.hpp"

////////////////////////////////////////////////////////////
using namespace gt;

const cPlugTag* cFigment::xPT_serialBuff = tOutline<cFigment>::makePlugTag("serial buffer");
const cPlugTag* cFigment::xPT_loadingParty = tOutline<cFigment>::makePlugTag("loading party");
const cPlugTag* cFigment::xPT_name = tOutline<cFigment>::makePlugTag("name");
const cPlugTag* cFigment::xPT_hash = tOutline<cFigment>::makePlugTag("hash");
const cPlugTag* cFigment::xPT_commands = tOutline<cFigment>::makePlugTag("commands");
const cPlugTag* cFigment::xPT_links = tOutline<cFigment>::makePlugTag("links");

const cCommand::dUID cFigment::xSave = tOutline<cFigment>::makeCommand(
	"save", &cFigment::patSave, cFigment::xPT_serialBuff,
	NULL
);

const cCommand::dUID cFigment::xLoad = tOutline<cFigment>::makeCommand(
	"load", &cFigment::patLoad,
	cFigment::xPT_serialBuff,
	cFigment::xPT_loadingParty,
	NULL
);

const cCommand::dUID cFigment::xGetName = tOutline<cFigment>::makeCommand(
	"get name", &cFigment::patGetName, cFigment::xPT_name,
	NULL
);

const cCommand::dUID cFigment::xGetHash = tOutline<cFigment>::makeCommand(
	"get hash", &cFigment::patGetHash, cFigment::xPT_hash,
	NULL
);

const cCommand::dUID cFigment::xGetCommands = tOutline<cFigment>::makeCommand(
	"get commands", &cFigment::patGetCommands, cFigment::xPT_commands,
	NULL
);

const cCommand::dUID cFigment::xGetLinks = tOutline<cFigment>::makeCommand(
	"get links", &cFigment::patGetLinks, cFigment::xPT_links,
	NULL
);


#if defined(DEBUG) && defined(GT_SPEED)
	const cPlugTag* cFigment::xPT_life = tOutline<cFigment>::makePlugTag("life");
	const cCommand::dUID cFigment::xTestJack = tOutline<cFigment>::makeCommand(
		"test jack", &cFigment::patTestJack, cFigment::xPT_life,
		NULL
	);

#endif

cFigment::cFigment()
:	mBlueprint(NULL), mSelf(NULL)
{
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

	tPlug<dNumVer> numVer;
	numVer = getVersion();
	numVer.save(pSaveHere);

	size_t start = pSaveHere->size();
	size_t sizePk = 0;
	dByte *buffSize = NULL;

	try{
		bpk::pack(&start, &buffSize, &sizePk);
		pSaveHere->add(buffSize, sizePk);

		for(dVersionPlugs::iterator itr = loadPattern.back().begin(); itr != loadPattern.back().end(); ++itr)
			itr->get().save(pSaveHere);

		size_t chunkSize = pSaveHere->size() - start;
		bpk::pack(&chunkSize, &buffSize, &sizePk);
		pSaveHere->overwrite(buffSize, sizePk, start);
		SAFEDEL_ARR(buffSize);

	}catch(...){
		delete [] buffSize;
		throw;
	}
}

void
cFigment::loadEat(cByteBuffer* pLoadFrom, dReloadMap *aReloads){
	dMigrationPattern loadPattern = getLoadPattern();

	if(loadPattern.empty())
		return;

	tPlug<dNumVer> numVer;
	numVer.loadEat(pLoadFrom);

	tPlug<size_t> chunkSize;
	chunkSize.loadEat(pLoadFrom);

	if(numVer.get() > loadPattern.size())
		throw excep::fromTheFuture(__FILE__, __LINE__);

	dVersionPlugs::iterator itrPrev, itrPrevEnd, itrCur;

	if(numVer.get() > 0){
		for(size_t idxVer = numVer.get()-1; idxVer < loadPattern.size(); ++idxVer){
			itrCur = loadPattern[idxVer].begin();
			if(idxVer==0){
				while(itrCur != loadPattern[0].end()){
					itrCur->get().loadEat( pLoadFrom, aReloads );
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

}

void
cFigment::jack(ptrLead pLead, cContext* pCon){
	PROFILE;

	pCon->startJackMode();
	start(pCon);
	try{
		ASRT_NOTNULL(mBlueprint);
		ASRT_NOTNULL(pCon);

#		ifdef GT_THREADS
			pLead->start(pCon->getSig());
			try{
				mBlueprint->getCom(pLead->mCom)->use(this, pLead);
			}catch(...){
				pLead->stop();
				throw;
			}
			pLead->stop();
#		else
			mBlueprint->getCom(pLead->mCom)->use(this, pLead);
#		endif

	}catch(excep::base_error &e){
		WARN_S(name() << e.what());

	}catch(...){
		UNKNOWN_ERROR;
	}
	stop(pCon);
}


void 
cFigment::run(cContext* pCon){
	//- Run these in case the child figment doesn't have a run function but has plugs to update.
	start(pCon);
	updatePlugs();
	work(pCon);
	stop(pCon);
}

void
cFigment::ini(cBlueprint *pBlue, tDirector<iFigment> *pSelf){
	mBlueprint = pBlue;
	mSelf = pSelf;
}

ptrFig
cFigment::getSmart(){
	ASRT_NOTNULL(mSelf);
	ptrFig rtnFig;
	rtnFig.linkDir(mSelf);
	return rtnFig;
}

void
cFigment::work(cContext* pCon){
}

cFigment::dMigrationPattern
cFigment::getLoadPattern(){
	return dMigrationPattern();
}

dStr const &
cFigment::requiredAddon() const {
	static const dStr noRequirement("");
	return noRequirement;
}

void 
cFigment::getLinks(std::list<ptrFig>* pOutLinks){
	DUMB_REF_ARG(pOutLinks);
}

void
cFigment::patSave(ptrLead aLead){
	ptrBuff buffer;

	aLead->assignTo(&buffer, xPT_serialBuff);

	save(buffer.get());
}

void
cFigment::patLoad(ptrLead aLead){
	ptrBuff buffer;

	aLead->assignTo(&buffer, xPT_serialBuff);

	loadEat(buffer.get());
}

void
cFigment::patGetName(ptrLead aLead){
	const dPlaChar *tmp = name();
	aLead->assignFrom(tmp, xPT_name);
}

void
cFigment::patGetHash(ptrLead aLead){
	dNameHash tmp = hash();
	aLead->assignFrom(tmp, xPT_hash);
}

void
cFigment::patGetCommands(ptrLead aLead){
	tPlugLinearContainer<cCommandContain, std::list> coms;
	dListComs tmp = gWorld.get()->getBlueprint(hash())->getAllComs();
	for(dListComs::iterator itr=tmp.begin(); itr != tmp.end(); ++itr)
		coms += cCommandContain(*itr);

	aLead->setPlug(&coms, xPT_commands);
}

void
cFigment::patGetLinks(ptrLead aLead){
	std::list<ptrFig> links;
	getLinks(&links);

	tPlugLinearContainer<ptrFig, std::list> plugs;
	plugs = links;
	aLead->setPlug(&plugs, xPT_links);
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
cWorldShutoff::work(cContext* pCon){
	gWorld.get()->mKeepLooping = false;
}

////////////////////////////////////////////////////////////

#ifdef GTUT
#	include "unitTestFigments.hpp"

GTUT_START(test_cfigment, test_suit){
	figmentTestSuit<cFigment>();
}GTUT_END;

GTUT_START(test_cWorldShutoff, test_suit){
	figmentTestSuit<cWorldShutoff>();
}GTUT_END;

GTUT_START(test_figment, polymorphNames){

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
	cFigment test;

	GTUT_ASRT(getHash<cFigment>()==test.hash(), "hashes don't match");
}GTUT_END;

class testMigration_v1: public cFigment{
private:
public:
	tPlug<int> mCat;

	testMigration_v1(){}
	virtual ~testMigration_v1(){}

	static const dPlaChar* identify(){ return "test migration figment"; }
	virtual const dPlaChar* name() const { return identify(); }
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

	static const dPlaChar* identify(){ return "test migration figment"; }
	virtual const dPlaChar* name() const { return identify(); }
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
	int timesJacked;

	testContextFigment() : refOther(NULL), throwOnRun(false), timesJacked(0) {}
	virtual ~testContextFigment() {}

	static const dPlaChar* identify(){ return "test context figment"; }
	virtual const dPlaChar* name() const { return identify(); }
	virtual dNameHash hash() const { return getHash<testContextFigment>(); }

	virtual void work(cContext* pCon){
		if(throwOnRun)
			throw excep::base_error(__FILE__, __LINE__);
		else
			if(refOther) refOther->run(currentCon);
	}

	virtual void jack(ptrLead pLead, cContext* pCon){
		pCon->startJackMode();
		start(pCon);
		try{
			if(refOther) refOther->jack(pLead, pCon);
		}catch(excep::base_error &e){
			DBUG_LO(e.what());
		}
		stop(pCon);
		++timesJacked;
	}

	bool stillStacked(){ return currentCon != NULL; }
};

GTUT_START(test_context, preventSelfReference){
	bool caughtLikeABoss = false;
	testContextFigment A, B;

	{
		cContext testMe;

		A.refOther = &B;
		B.refOther = &A;
		try{
			A.run(&testMe);
		}catch(excep::stackFault_selfReference){
			caughtLikeABoss = true;
		}
		B.stop(&testMe);
		A.stop(&testMe);
	}

	GTUT_ASRT(caughtLikeABoss, "Didn't prevent self reference.");

	gt::gWorld.get()->flushLines();	//- So we clear out warnings.
}GTUT_END;

GTUT_START(test_context, forceUnwind){
	testContextFigment A, B;
	bool caughtLikeABoss = false;

	A.refOther = &B;
	B.throwOnRun = true;

	try{
		cContext testMe;
		A.run(&testMe);
	}catch(excep::base_error){
		caughtLikeABoss = true;
	}

	GTUT_ASRT(caughtLikeABoss, "Didn't throw.");

	gt::gWorld.get()->flushLines();	//- So we clear out warnings.
}GTUT_END;

GTUT_START(test_context, allowJackStack){
	testContextFigment A, B;
	ptrLead testLead;

	A.refOther = &B;
	{
		cContext testMe;
		B.start(&testMe);	//- started in run mode.
		A.jack(testLead, &testMe);
		B.stop(&testMe);
	}
	GTUT_ASRT(A.timesJacked == 1, "A not stacked");
	GTUT_ASRT(B.timesJacked == 1, "B not stacked");
}GTUT_END;

GTUT_START(test_context, preventDoubleJackStack){
	testContextFigment A, B;
	ptrLead testLead;

	A.refOther = &B;
	B.refOther = &A;

	{
		cContext testMe;
		A.jack(testLead, &testMe);
	}

	GTUT_ASRT(A.timesJacked == 1, "A not stacked right.");
	GTUT_ASRT(B.timesJacked == 1, "B not stacked right.");

	gt::gWorld.get()->flushLines();	//- So we clear out warnings.

}GTUT_END;

#endif
