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

#include "figment.hpp"	//- so we get all implementation.


////////////////////////////////////////////////////////////
// Globals

const char *MSG_UNKNOWN_ERROR = "unknown error";

gt::tMrSafety<gt::cWorld> gt::gWorld;


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

bool gt::cWorld::thereCanBeOnlyOne = false;

cWorld::cWorld():
	mKeepLooping(true),
	mBicycleSetup(false)
{
	if(thereCanBeOnlyOne)
		THROW_ERROR("can only create the world once");

	thereCanBeOnlyOne = true;

	mVillageBicycle = ptrFig(new cEmptyFig());
	mRoot = ptrFig(new cWorldShutoff());

	(void)primordial::makeProfileToken(__FILE__, __LINE__); //- Ensure it exists.
	mProfiler = primordial::xProfiler;

	primordial::lo("World created."); //- Ensure it exists.
	mLines = primordial::xLines;

#	ifdef GT_THREADS
		mProfileGuard = primordial::xProfileGuard;
		mLineGuard = primordial::xLineGuard;
#	endif
}

cWorld::~cWorld(){
	try{
		mRoot.redirect(NULL);
		mVillageBicycle.redirect(NULL);

		while(!mBlueprints.empty()){
			mBlueprints.begin()->second.mBlueprint->mCleanup();
			mBlueprints.erase( mBlueprints.begin() );
		}

		primordial::cleanup();

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
		std::list<ptrFig> linksA, linksB;
		std::list<ptrFig>*	branches = &linksA;
		std::list<ptrFig>*	prev = &linksB;
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
		THROW_ERROR("Can't make figment as the provided name hash isn't drafted.");

	return mScrBMapItr->second.mBlueprint->make();
}

ptrFig
cWorld::makeFig(const dPlaChar *pName){
	return makeFig(makeHash(toNStr(pName)));
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

dConSig
cWorld::regContext(cContext* pCon){
	return mContexts.add(pCon);
}

void
cWorld::unregContext(dConSig pSig){
	mContexts.del(pSig);
}

bool
cWorld::activeContext(dConSig pSig){
	return mContexts.valid(pSig);
}

void
cWorld::lazyCloseAddon(const dPlaChar* pAddonName){
	for(std::list<dStr>::iterator itr = mAddonsToClose.begin(); itr != mAddonsToClose.end(); ++itr)
		if(itr->compare(pAddonName)==0)
			return;

	mAddonsToClose.push_back(pAddonName);
}

const cPlugTag* 
cWorld::getPlugTag(dNameHash pFigHash, cPlugTag::dUID pPTHash){
	PROFILE;

	mScrBMapItr =  mBlueprints.find(pFigHash);

	if(mScrBMapItr == mBlueprints.end())
		THROW_ERROR("figment wasn't found");

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


////////////////////////////////////////////////////////////
// primordial

using namespace gt;

//- Don't assign anything to the stuff below.
cWorld::dLines* gt::cWorld::primordial::xLines;
cProfiler* gt::cWorld::primordial::xProfiler;

#ifdef GT_THREADS
	boost::recursive_mutex *gt::cWorld::primordial::xProfileGuard;
	boost::recursive_mutex *gt::cWorld::primordial::xLineGuard;
#endif


void
cWorld::primordial::lo(const dStr& pLine, bool cleanup){
	static bool linesSetup = false;
	if(!linesSetup){
		if(cleanup)	//- nothing to do.
			return;

		linesSetup = true;
		xLines = new dLines();
#		ifdef GT_THREADS
			xLineGuard = new boost::recursive_mutex();
#		endif
	}

	if(cleanup){
		if(!linesSetup)
			return;

		{
#			ifdef GT_THREADS
				boost::lock_guard<boost::recursive_mutex> lock(*xLineGuard);
#			endif
			SAFEDEL(xLines);
			linesSetup = false;
		}
#		ifdef GT_THREADS
			SAFEDEL(xLineGuard);
#		endif

	}else if(!pLine.empty()){
#		ifdef GT_THREADS
			boost::lock_guard<boost::recursive_mutex> lock(*xLineGuard);
#		endif

		xLines->push_back(pLine);
	}
}

void
cWorld::primordial::warnError(const char *msg, const char* pFile, const unsigned int pLine){
	std::stringstream ss;
	ss << "<!> Warning detected in file '" << pFile << "' on line " << pLine << std::endl << ". Info: " << msg;
	lo(ss.str());
}

void
cWorld::primordial::warnError(excep::base_error &pE, const char* pFile, const unsigned int pLine){
	warnError(pE.what(), pFile, pLine);
}

cProfiler::cToken
cWorld::primordial::makeProfileToken(const char* pFile, unsigned int pLine, bool cleanup){
	static bool profileSetup = false;
	if(!profileSetup){
		if(!cleanup){	//- Nothing to do.
			profileSetup = true;
			xProfiler = new cProfiler();
#			ifdef GT_THREADS
				xProfileGuard = new boost::recursive_mutex();
#			endif
		}
	}

	if(cleanup){
		if(profileSetup){
			{
#				ifdef GT_THREADS
					boost::lock_guard<boost::recursive_mutex> lock(*xProfileGuard);
#				endif
				SAFEDEL(xProfiler);
				profileSetup = false;
			}
#			ifdef GT_THREADS
				SAFEDEL(xProfileGuard);
#			endif
		}
	}else if(pFile != NULL){
#		ifdef GT_THREADS
			boost::lock_guard<boost::recursive_mutex> lock(*xProfileGuard);
#		endif
		return xProfiler->makeToken(pFile, pLine);
	}

	return cProfiler::cToken(NULL, 0, 0);
}

void
cWorld::primordial::makeProfileReport(std::ostream &log){
#	ifdef GT_THREADS
		if(xProfileGuard==NULL)
			return;

		boost::lock_guard<boost::recursive_mutex> lock(*xProfileGuard);
#	endif
	(void)makeProfileToken(__FILE__, __LINE__); //- Ensure it exists.
	xProfiler->flushThatLog(log);
}

#ifdef GTUT
	bool gt::cWorld::primordial::mSuppressError = false;

	void
	gt::cWorld::primordial::suppressNextError(){
		mSuppressError = true;
	}
#endif

void
cWorld::primordial::redirectWorld(cWorld* pWorldNew){
	if(pWorldNew){

		if(xLines == pWorldNew->mLines || xProfiler == pWorldNew->mProfiler)
			return;

#		ifdef GT_THREADS
			boost::lock_guard<boost::recursive_mutex> lockA(*xLineGuard);
			boost::lock_guard<boost::recursive_mutex> lockB(*xProfileGuard);
#		endif

		{
#			ifdef GT_THREADS
				boost::lock_guard<boost::recursive_mutex> lockC(*pWorldNew->mProfileGuard);
				boost::lock_guard<boost::recursive_mutex> lockD(*pWorldNew->mLineGuard);
#			endif

			lo("");	//- This shouldn't add a line, but it ensures it's initialised.
			(void)makeProfileToken(NULL, 0);	//- same.

			if(!xLines->empty())
				pWorldNew->mLines->splice(pWorldNew->mLines->end(), *xLines);

			*pWorldNew->mProfiler += *xProfiler;

			delete xLines;
			delete xProfiler;

			xLines = pWorldNew->mLines;
			xProfiler = pWorldNew->mProfiler;
		}

#		ifdef GT_THREADS
			xProfileGuard = pWorldNew->mProfileGuard;
			xLineGuard = pWorldNew->mLineGuard;
#		endif

		gWorld.take(pWorldNew);	//- Old world cleaned up by doing this.

	}else{
		gWorld.drop();
	}

}

void
cWorld::primordial::cleanup(){
	lo("", true);
	(void)makeProfileToken(NULL, 0, true);
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
	virtual dStr const& requiredAddon() const { DONT_USE_THIS; };
	static dNameHash extends(){ return getHash<testDraftParent>(); }
	virtual dNameHash getExtension() const { return extends(); }

	virtual ~testDraftChild(){}
};

class testDraftReplace: public testDraftParent{
public:
	static const dPlaChar* identify(){ return "test draft replace"; }
	virtual const dPlaChar* name() const { return identify(); };

	virtual dNameHash hash() const { return getHash<testDraftReplace>(); };

	virtual dNumVer getVersion() const { return 0; }
	virtual dStr const& requiredAddon() const { DONT_USE_THIS; };
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

