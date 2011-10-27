#include "world.hpp"
#include "figment.hpp"

using namespace gt;

////////////////////////////////////////////////////////////
// Globals and statics
using namespace gt;

tMrSafety<cWorld> gt::gWorld;

//- Don't assign anything to the stuff below.
cWorld::dLines* cWorld::xLines;
cProfiler* cWorld::xProfiler;

#ifdef GT_THREADS
	boost::recursive_mutex *cWorld::xProfileGuard;
	boost::recursive_mutex *cWorld::xLineGuard;
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
		throw excep::base_error("can only create the world once", __FILE__, __LINE__);

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
	#endif
}

cWorld::~cWorld(){
	{
		(void)makeProfileToken(__FILE__, __LINE__); //- Ensure it exists.
	}
	delete xProfiler;

	//- Be super careful that we don't try and profile anything anymore.
	lo("end of the world"); //- Ensure it exists.
	flushLines();
	delete xLines;

	mRoot.redirect(NULL);
	mVillageBicycle.redirect(NULL);
}

void
cWorld::addBlueprint(cBlueprint* pAddMe){
	PROFILE;

	// Archive the old blueprint being replaced.
	if( pAddMe->replace() != uDoesntReplace ){
		mScrBMapItr = mBlueprints.find(pAddMe->replace());
		if(mScrBMapItr != mBlueprints.end()){
			mBlueArchive[ mScrBMapItr->first ] = sBlueprintHeader(mScrBMapItr->second.mBlueprint, mScrBMapItr->first);
			DBUG_LO("Blueprint '" << pAddMe->name() << "' replaced '" << mScrBMapItr->second.mBlueprint->name() << "'");
		}

		mBlueprints[pAddMe->replace()] = sBlueprintHeader( pAddMe, pAddMe->replace() );		
	}

	// Even if this figment replaces another, it still appears under its own name hash.
	if( mBlueprints.find(pAddMe->hash()) == mBlueprints.end()){	//new blueprint
		mBlueprints[pAddMe->hash()] = sBlueprintHeader( pAddMe, uDoesntReplace );		
		DBUG_LO("Blueprint '" << pAddMe->name() << "' added to library.");
		
	}else{
		DBUG_LO("Blueprint '" <<  pAddMe->name() 
			<< "' with hash "<< pAddMe->name()
			<< ", has already been added"
		);
	}
}

const cBlueprint*
cWorld::getBlueprint(dNameHash pNameHash){
	PROFILE;

	mScrBMapItr = mBlueprints.find(pNameHash);
	if(mScrBMapItr == mBlueprints.end())
		throw excep::base_error("bad name hash", __FILE__, __LINE__);

	return mScrBMapItr->second.mBlueprint;
}

void
cWorld::removeBlueprint(const cBlueprint* pRemoveMe){
	PROFILE;

	DBUG_LO("Erasing blueprint '" << pRemoveMe->name() << "'");

	//- Some figments should never be remove.
	if(
		pRemoveMe->hash() == getHash<cFigment>()
		|| pRemoveMe->hash() == getHash<cEmptyFig>()
	)
		return;

	//- If this figment replaced another, restore the original blueprint.
	if(pRemoveMe->replace() != uDoesntReplace){
		mScrBMapItr = mBlueprints.find(pRemoveMe->replace());
		if(mScrBMapItr != mBlueprints.end()){
			mScrBMapItr = mBlueArchive.find(mScrBMapItr->first);
			if(mScrBMapItr != mBlueArchive.end()){
				mBlueprints[pRemoveMe->replace()] = mScrBMapItr->second;
				DBUG_LO("Blueprint '" << mScrBMapItr->second.mBlueprint->name() << "' restored");
			}
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

		//- Empty the figments
		for(
			std::map<iFigment*, ptrFig>::iterator itr = figs.begin();
			itr != figs.end();
			++itr
		){
			DBUG_VERBOSE_LO( "Emptying figment " << reinterpret_cast<unsigned int>(itr->first) );

			itr->second.redirect(getEmptyFig());
		}

		//- Now erase the blueprint.
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
	#endif
}

ptrLead
cWorld::makeLead(cCommand::dUID pComID, dConSig pConx){
	PROFILE;
	ptrLead rtnLead(new cLead( pComID, pConx ));
	return rtnLead;
}

ptrLead
cWorld::makeLead(const dNatChar *aFigName, const dNatChar *aComName, dConSig aConx){
	dStr tmpStr = aFigName;
	tmpStr.append(aComName);
	dNameHash hash = makeHash(tmpStr.c_str());
	ptrLead rtnLead(new cLead( hash, aConx ));
	return rtnLead;
}

const cPlugTag* 
cWorld::getPlugTag(dNameHash pFigHash, cPlugTag::dUID pPTHash){
	PROFILE;

	mScrBMapItr =  mBlueprints.find(pFigHash);

	if(mScrBMapItr == mBlueprints.end())
		throw excep::base_error("figment wasn't found", __FILE__, __LINE__);

	return mScrBMapItr->second.mBlueprint->getPlugTag(pPTHash);
}

const cPlugTag*
cWorld::getPlugTag(const dNatChar *figName, const dNatChar *tagName){
	return getPlugTag(makeHash(figName), makeHash(tagName));
}

const cPlugTag*
cWorld::getPlugTag(cPlugTag::dUID aID){
	PROFILE;

	for(dBlueprintMap::iterator bp = mBlueprints.begin(); bp != mBlueprints.end(); ++bp){
		if( bp->second.mBlueprint->hasPlugTag(aID) )
			return bp->second.mBlueprint->getPlugTag(aID);
	}
	throw excep::notFound("plug tag", __FILE__, __LINE__);
}

void
cWorld::setRoot(ptrFig pNewRoot){
	mRoot = pNewRoot;
}

void
cWorld::makeProfileReport(std::ostream &log){
	xProfiler->flushThatLog(log);
}

void
cWorld::warnError(excep::base_error &pE, const char* pFile, const unsigned int pLine){
	std::stringstream ss;
	ss << "!Warning detected in file " << pFile << " on line " << pLine << std::endl << "	" << pE.what();
	lo(ss.str());
}

void
cWorld::warnError(const char *msg, const char* pFile, const unsigned int pLine){
	std::stringstream ss;
	ss << "!Warning detected in file " << pFile << " on line " << pLine << std::endl << "	" << msg;
	lo(ss.str());
}

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
class testDraftParent: public cFigContext, private tOutline<testDraftParent>{
public:
	static const cPlugTag*	xPT_A;
	static const cCommand::dUID	xCommandA;

	static const char* identify(){ return "test draft parent"; }
	virtual const char* name() const { return identify(); }

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

	void patA(ptrLead aLead){

	}
};

const cPlugTag*	testDraftParent::xPT_A = tOutline<testDraftParent>::makePlugTag("A");
const cCommand::dUID testDraftParent::xCommandA = tOutline<testDraftParent>::makeCommand(
	"command A", &testDraftParent::patA, testDraftParent::xPT_A,
	NULL
);

//- Just extends the parent.
class testDraftChild: public testDraftParent, private tOutline<testDraftChild>{
public:
	static const char* identify(){ return "test draft child"; }
	virtual const char* name() const { return identify(); };

	virtual dNameHash hash() const { return getHash<testDraftChild>(); };

	static dNameHash extends(){ return getHash<testDraftParent>(); }
	virtual dNameHash getExtension() const { return extends(); }
};

class testDraftReplace: public testDraftParent, private tOutline<testDraftReplace>{
public:
	static const char* identify(){ return "test draft replace"; }
	virtual const char* name() const { return identify(); };
	virtual dNameHash hash() const { return getHash<testDraftReplace>(); };
	static dNameHash replaces(){ return getHash<testDraftParent>(); }
	virtual dNameHash getReplacement() const { return replaces(); };
};

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

