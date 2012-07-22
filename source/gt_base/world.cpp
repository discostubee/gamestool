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

#include "world.hpp"
#include "figment.hpp"

using namespace gt;

////////////////////////////////////////////////////////////
// Globals and statics
using namespace gt;

const char *MSG_UNKNOWN_ERROR = "unknown error";

tMrSafety<cWorld> gt::gWorld;

//- Don't assign anything to the stuff below.
cWorld::dLines* cWorld::xLines;
cProfiler* cWorld::xProfiler;

#ifdef GT_THREADS
	boost::recursive_mutex *cWorld::xProfileGuard;
	boost::recursive_mutex *cWorld::xLineGuard;

	#ifdef GTUT
		boost::recursive_mutex *cWorld::xSuppressGuard;
	#endif
#endif

bool cWorld::thereCanBeOnlyOne = false;

////////////////////////////////////////////////////////////
// Blueprint stuff
using namespace gt;

//!\brief	Used to keep track of the things which may have been replaced by this blueprint.
struct cWorld::sBlueprintHeader{
	cBlueprint* mBlueprint;
	dNameHash mReplaced;

	sBlueprintHeader():
		mReplaced(uDoesntReplace)
	{}

	sBlueprintHeader(cBlueprint* pBlue, dNameHash pName):
		mBlueprint(pBlue), mReplaced(pName)
	{}

	~sBlueprintHeader()
	{}

	//sBlueprintHeader operator = (const sBlueprintHeader* pCopyMe){
	//	const_cast<cBlueprint*>(mBlueprint) = pCopyMe->mBlueprint;
	//	mReplaced = pCopyMe->mReplaced;
	//	return *this;
	//}
};

////////////////////////////////////////////////////////////
// World

using namespace gt;

void
cWorld::lo(const dStr& pLine){
	static bool linesSetup = false;
	if(!linesSetup){
		linesSetup = true;
		xLines = new dLines();
		#ifdef GT_THREADS
			xLineGuard = new boost::recursive_mutex();
		#endif
	}
#ifdef GT_THREADS
	boost::lock_guard<boost::recursive_mutex> lock(*xLineGuard);
#endif
	xLines->push_back(pLine);
}

cProfiler::cToken
cWorld::makeProfileToken(const char* pFile, unsigned int pLine){
	static bool profileSetup = false;
	if(!profileSetup){
		profileSetup = true;
		xProfiler = new cProfiler();
		#ifdef GT_THREADS
			xProfileGuard = new boost::recursive_mutex();
		#endif
	}
#ifdef GT_THREADS
	boost::lock_guard<boost::recursive_mutex> lock(*xProfileGuard);
#endif
	return xProfiler->makeToken(pFile, pLine);
}

dConSig
cWorld::regContext(cContext* aCon){
	return mContexts.add(aCon);
}

void
cWorld::unregContext(dConSig aConx){
	mContexts.del(aConx);
}

cWorld::cWorld():
	mKeepLooping(true),
	mBicycleSetup(false)
{
	if(thereCanBeOnlyOne)
		THROW_BASEERROR("can only create the world once");

	thereCanBeOnlyOne = true;

	mVillageBicycle = ptrFig(new cEmptyFig());
	mRoot = ptrFig(new cWorldShutoff());

	(void)makeProfileToken(__FILE__, __LINE__); //- Ensure it exists.
	mProfiles = xProfiler;

	lo("World created."); //- Ensure it exists.
	mLines = xLines;

	#ifdef GT_THREADS
		//- Used so external modules can use the location
		mProfileGuard = xProfileGuard;
		mLineGuard = xLineGuard;

		#ifdef GTUT
			mSuppressGuard = xSuppressGuard;
		#endif
	#endif
}

cWorld::~cWorld(){
	try{
		mRoot.redirect(NULL);
		mVillageBicycle.redirect(NULL);

		for(std::list<dStr>::iterator itr = mAddonsToClose.begin(); itr != mAddonsToClose.end(); ++itr)
			closeAddon(*itr);

		while(!mBlueprints.empty()){
			mBlueprints.begin()->second.mBlueprint->mCleanup();
			mBlueprints.erase( mBlueprints.begin() );
		}

		(void)makeProfileToken(__FILE__, __LINE__); //- Ensure it exists.
		delete xProfiler;

		//- Be super careful that we don't try and profile anything anymore.
		lo("end of the world"); //- Ensure it exists.
		flushLines();
		delete xLines;

	}catch(...){
	}
}

void
cWorld::addBlueprint(cBlueprint* pAddMe){
	PROFILE;

	// Even if this figment replaces another, it still appears under its own name hash.
	if( mBlueprints.find(pAddMe->hash()) == mBlueprints.end()){	//new blueprint
		mBlueprints[pAddMe->hash()] = sBlueprintHeader( pAddMe, uDoesntReplace );
		DBUG_LO("Blueprint '" << pAddMe->name() << "' added to library");

	}else{
		DBUG_LO("Blueprint '" <<  pAddMe->name() << "', has already been added");
		return;
	}

	if( pAddMe->replace() != uDoesntReplace ){
		mScrBMapItr = mBlueprints.find(pAddMe->replace());
		if(mScrBMapItr != mBlueprints.end()){
			mBlueArchive[ mScrBMapItr->first ] = sBlueprintHeader(mScrBMapItr->second.mBlueprint, mScrBMapItr->first);	// Archive the old blueprint being replaced.

			mScrBMapItr->second = sBlueprintHeader( pAddMe, mScrBMapItr->second.mReplaced );
			DBUG_VERBOSE_LO("Blueprint '" << pAddMe->name() << "' replaced '" << mBlueArchive[ mScrBMapItr->first ].mBlueprint->name() << "'");
		}else{
			WARN_S(pAddMe->name() << " missing parent");
		}
	}
}

const cBlueprint*
cWorld::getBlueprint(dNameHash pNameHash){
	PROFILE;

	mScrBMapItr = mBlueprints.find(pNameHash);
	if(mScrBMapItr == mBlueprints.end())
		throw excep::base_error("couldn't find blueprint", __FILE__, __LINE__);

	return mScrBMapItr->second.mBlueprint;
}

void
cWorld::removeBlueprint(const cBlueprint* pRemoveMe){
	PROFILE;

	DBUG_LO("Erasing blueprint '" << pRemoveMe->name());

	//- If this figment replaced another, restore the original blueprint.
	if(pRemoveMe->replace() != uDoesntReplace){
		mScrBMapItr = mBlueprints.find(pRemoveMe->replace());
		if(mScrBMapItr != mBlueprints.end()){
			mScrBMapItr = mBlueArchive.find(mScrBMapItr->first);
			if(mScrBMapItr != mBlueArchive.end()){
				mBlueprints[pRemoveMe->replace()] = mScrBMapItr->second;
				DBUG_LO("Blueprint '" << mScrBMapItr->second.mBlueprint->name() << "' restored. Archive entry removed");
			}
			mBlueArchive.erase(mScrBMapItr);
		}
	}

	mScrBMapItr = mBlueprints.find(pRemoveMe->hash());
	if(mScrBMapItr != mBlueprints.end()){
		std::list<ptrFig>*	branches = new std::list<ptrFig>();
		std::list<ptrFig>*	prev = new std::list<ptrFig>();
		std::map<iFigment*, ptrFig> figs;

		//- Find and empty any objects using this blueprint.
		mRoot->getLinks(branches);
		prev->push_back(mRoot);

		do{ //- while there are branches still left to explore.
			std::list<ptrFig>* temp = prev;
			prev = branches;
			branches = temp;
			branches->clear();

			for(
				std::list<ptrFig>::iterator i = prev->begin();
				i != prev->end();
				++i
			){
				if( figs.find( i->get() ) == figs.end() ){ // first time funny.
					(*i)->getLinks( branches );	//add to the next series of branches.

					if( i->get()->hash() == pRemoveMe->hash() ){
						figs[i->get()] = *i;
					}
				}
			}

		}while(branches->size() > 0);

		for(	//- Empty the figments
			std::map<iFigment*, ptrFig>::iterator itr = figs.begin();
			itr != figs.end();
			++itr
		){
			DBUG_VERBOSE_LO( "Emptying figment " << itr->first->name() );

			itr->second.redirect(getEmptyFig());
		}

		mBlueprints.erase(mScrBMapItr);
	}
}

ptrFig
cWorld::makeFig(dNameHash pNameHash){
	PROFILE;

	mScrBMapItr = mBlueprints.find(pNameHash);

	if(mScrBMapItr == mBlueprints.end())
		throw excep::base_error("Can't make figment as the provided name hash isn't drafted.", __FILE__, __LINE__);

	return mScrBMapItr->second.mBlueprint->make();
}

ptrFig
cWorld::makeFig(const dPlaChar *pName){
	return makeFig(makeHash(toNStr(pName)));
}

void
cWorld::copyWorld(cWorld* pWorld){
	if(!pWorld->mLines->empty())
		mLines->splice(mLines->end(), *pWorld->mLines);

	*mProfiles += *pWorld->mProfiles;

	#ifdef GT_THREADS
		mProfileGuard = pWorld->mProfileGuard;
		mLineGuard = pWorld->mLineGuard;
		xProfileGuard = pWorld->mProfileGuard;
		xLineGuard = pWorld->mLineGuard;

		#ifdef GTUT
			xSuppressGuard = pWorld->xSuppressGuard;
			mSuppressGuard = pWorld->mSuppressGuard;
		#endif
	#endif
}

void
cWorld::lazyCloseAddon(const dStr &name){
	for(
		std::list<dStr>::iterator itr = mAddonsToClose.begin();
		itr != mAddonsToClose.end();
		++itr
	){
		if(itr->compare(name)==0)
			return;
	}

	mAddonsToClose.push_back(name);
}

ptrLead
cWorld::makeLead(cCommand::dUID pComID){
	PROFILE;
	ptrLead rtnLead(new cLead(pComID));
	return rtnLead;
}

ptrLead
cWorld::makeLead(const dPlaChar *aFigName, const dPlaChar *aComName){
	dNatStr totalString = toNStr(aFigName);
	totalString.t.append( toNStr(aComName) );
	dNameHash hash = makeHash(totalString);
	ptrLead rtnLead(new cLead(hash));
	return rtnLead;
}

const cPlugTag* 
cWorld::getPlugTag(dNameHash pFigHash, cPlugTag::dUID pPTHash){
	PROFILE;

	mScrBMapItr =  mBlueprints.find(pFigHash);

	if(mScrBMapItr == mBlueprints.end())
		THROW_BASEERROR("figment wasn't found");

	return mScrBMapItr->second.mBlueprint->getPlugTag(pPTHash);
}

const cPlugTag*
cWorld::getPlugTag(const dNatChar *figName, const dNatChar *tagName){
	return getPlugTag(
		makeHash(toNStr(figName)),
		makeHash(toNStr(tagName))
	);
}

const cPlugTag*
cWorld::getPlugTag(cPlugTag::dUID aID){
	PROFILE;

	const cPlugTag* tmp=NULL;

	for(
		dBlueprintMap::iterator bp = mBlueprints.begin();
		bp != mBlueprints.end() && tmp==NULL;
		++bp
	){
		tmp = bp->second.mBlueprint->getPlugTag(aID);
	}

	if(tmp==NULL)
		throw excep::notFound("plug tag", __FILE__, __LINE__);

	return tmp;
}

void
cWorld::setRoot(ptrFig pNewRoot){
	mRoot = pNewRoot;
}

void
cWorld::makeProfileReport(std::ostream &log){
	{
		(void)makeProfileToken(__FILE__, __LINE__); //- Ensure it exists.
	}
	xProfiler->flushThatLog(log);
}

void
cWorld::warnError(const char *msg, const char* pFile, const unsigned int pLine){
	#ifdef GTUT
		if(mSuppressError){
			mSuppressError = false;
			return;
		}
	#endif
	std::stringstream ss;
	ss << "!Warning detected in file " << pFile << " on line " << pLine << std::endl << "	" << msg;
	lo(ss.str());
}

void
cWorld::warnError(excep::base_error &pE, const char* pFile, const unsigned int pLine){
	warnError(pE.what(), pFile, pLine);
}

#ifdef GTUT
	bool cWorld::mSuppressError = false;

	void
	cWorld::suppressNextError(){
		#ifdef GT_THREADS
			static bool setup = false;
			if(!setup){
				setup = true;
				xSuppressGuard = new boost::recursive_mutex();
			}

			boost::lock_guard<boost::recursive_mutex> lock(*xSuppressGuard);
		#endif
		mSuppressError = true;
	}
#endif

ptrFig
cWorld::getEmptyFig(){
	if(mBicycleSetup == false){
		if(mBlueprints.find(getHash<cEmptyFig>()) == mBlueprints.end()){
			tOutline<cEmptyFig>::draft();
		}
		mBicycleSetup = true;
	}

	return mVillageBicycle;
}

const cWorld::dContextLookup&
cWorld::getContextLookup(){
	return mContexts;
}

void
cWorld::flushLines(){
	for(dLines::iterator i = xLines->begin(); i != xLines->end(); ++i){
		std::cout << (*i) << std::endl;
	}
	xLines->clear();
}

////////////////////////////////////////////////////////////
// Functions

void
gt::redirectWorld(cWorld* pWorldNew){
	if(pWorldNew){
		cWorld* temp = new cWorld();	// We need the member pointers to the statics to exist.
		//std::cout << (long)gt::gWorld << " vs " << (long)pWorldNew << std::endl; //!!!
		//std::cout << "xLines at " << (long)(&gt::cWorld::xLines) << std::endl; //!!!
		//std::cout << (long)gt::cWorld::xLines.get() << " vs " << (long)pWorldNew->mLines << std::endl; //!!!
		pWorldNew->copyWorld(temp);
		delete temp;

		cWorld::xLines = pWorldNew->mLines;
		cWorld::xProfiler = pWorldNew->mProfiles;
		gWorld.take(pWorldNew);
	}else{
		gWorld.drop();
	}

}

////////////////////////////////////////////////////////////

#ifdef GTUT

//- A basic class to test out some functions of the world.
class testDraftParent: public cFigment{
public:
	static const cPlugTag*	xPT_A;
	static const cCommand::dUID	xCommandA;

	static const dPlaChar* identify(){ return "test draft parent"; }
	virtual const dPlaChar* name() const { return identify(); }

	virtual dNameHash hash() const { return getHash<testDraftParent>(); }

	static dNameHash replaces(){ return uDoesntReplace; }
	virtual dNameHash getReplacement() const { return replaces(); }

	static dNameHash extends(){ return uDoesntExtend; }
	virtual dNameHash getExtension() const { return extends(); }

	virtual void jack(ptrLead pLead, cContext* pCon) {}
	virtual void run(cContext* pCon) {}
	virtual void save(cByteBuffer* pAddHere) {}
	virtual void loadEat(cByteBuffer* pBuff, dReloadMap *aReloads = NULL) {}
	virtual void getLinks(std::list<ptrFig>* pOutLinks) {}
	virtual dMigrationPattern getLoadPattern() { return dMigrationPattern(); }
	virtual dNumVer getVersion() const { return 0; }

	virtual ~testDraftParent(){}

protected:
	void patA(ptrLead aLead){
	}
};

const cPlugTag*	testDraftParent::xPT_A = tOutline<testDraftParent>::makePlugTag("A");

const cCommand::dUID testDraftParent::xCommandA = tOutline<testDraftParent>::makeCommand(
	"command A", &testDraftParent::patA, testDraftParent::xPT_A,
	NULL
);

//- Just extends the parent.
class testDraftChild: public testDraftParent{
public:
	static const dPlaChar* identify(){ return "test draft child"; }
	virtual const dPlaChar* name() const { return identify(); };

	virtual dNameHash hash() const { return getHash<testDraftChild>(); };

	virtual dNumVer getVersion() const { return 0; }

	static dNameHash extends(){ return getHash<testDraftParent>(); }
	virtual dNameHash getExtension() const { return extends(); }

	virtual ~testDraftChild(){}
};

class testDraftReplace: public testDraftParent{
public:
	static const dPlaChar* identify(){ return "test draft replace"; }
	virtual const dPlaChar* name() const { return identify(); };

	virtual dNameHash hash() const { return getHash<testDraftReplace>(); };

	static dNameHash replaces(){ return getHash<testDraftParent>(); }
	virtual dNameHash getReplacement() const { return replaces(); };

	virtual ~testDraftReplace(){}
};

GTUT_START(test_figInterface, getSmart){
	tOutline<cFigment>::draft();

	ptrFig testMe = gWorld.get()->makeFig(getHash<cFigment>());
	ptrFig testSmart = testMe->getSmart();
	GTUT_ASRT(testSmart->hash() == getHash<cFigment>(), "didn't get the same figment");
}GTUT_END;

GTUT_START(test_world, drafting){
	tOutline<testDraftParent>::draft();
	tOutline<testDraftChild>::draft();
	GTUT_ASRT(gWorld.get()->makeFig(getHash<testDraftParent>())->hash() == getHash<testDraftParent>(), "didn't make the right figment");
	GTUT_ASRT(gWorld.get()->makeFig(getHash<testDraftChild>())->hash() == getHash<testDraftChild>(), "didn't make the right figment");

	tOutline<testDraftReplace>::draft();
	GTUT_ASRT(gWorld.get()->makeFig(getHash<testDraftReplace>())->hash() == getHash<testDraftReplace>(), "didn't make the right figment");
	GTUT_ASRT(gWorld.get()->makeFig(getHash<testDraftParent>())->hash() == getHash<testDraftReplace>(), "didn't make the right figment");
}GTUT_END;

GTUT_START(test_world, extend_commands){
	gWorld.get()->getBlueprint(getHash<testDraftChild>())->getCom(testDraftParent::xCommandA);
}GTUT_END;

GTUT_START(test_world, extend_plugTags){

}GTUT_END;

#endif

