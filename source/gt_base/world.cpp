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
	int mTimesAdded;

	sBlueprintHeader():
		mBlueprint(NULL), mReplaced(uDoesntReplace), mTimesAdded(0)
	{}

	sBlueprintHeader(cBlueprint* pBlue, dNameHash pName):
		mBlueprint(pBlue), mReplaced(pName), mTimesAdded(0)
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
	mKeepLooping(true)
{
	if(thereCanBeOnlyOne)
		THROW_ERROR("can only create the world once");

	thereCanBeOnlyOne = true;

	//- Assume these figments will have their blueprints managed properly later.
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
		for(mScrBMapItr = mBlueprints.begin(); mScrBMapItr != mBlueprints.end(); ++mScrBMapItr)
			mScrBMapItr->second.mBlueprint->mCleanup();

		for(mScrBMapItr = mBlueArchive.begin(); mScrBMapItr != mBlueArchive.end(); ++mScrBMapItr)
			mScrBMapItr->second.mBlueprint->mCleanup();

		mRoot.redirect(NULL);
		mVillageBicycle.redirect(NULL);

		primordial::cleanup();

	}catch(...){
	}
}

void
cWorld::addBlueprint(cBlueprint* pAddMe){
	PROFILE;

	mScrBMapItr = mBlueprints.find(pAddMe->hash());
	if(mScrBMapItr == mBlueprints.end()){
		// Even if this figment replaces another, it still appears under its own name hash.
		mBlueprints[pAddMe->hash()] = sBlueprintHeader( pAddMe, uDoesntReplace );

		if( pAddMe->replace() != uDoesntReplace ){
			mScrBMapItr = mBlueprints.find(pAddMe->replace());
			if(mScrBMapItr != mBlueprints.end()){
				mBlueArchive[ mScrBMapItr->first ] = sBlueprintHeader(mScrBMapItr->second.mBlueprint, mScrBMapItr->first);	// Archive the old blueprint being replaced.
				mScrBMapItr->second = sBlueprintHeader( pAddMe, mScrBMapItr->second.mReplaced );

				DBUG_LO("Blueprint '" << pAddMe->name() << "' replaced '" << mBlueArchive[ mScrBMapItr->first ].mBlueprint->name() << "'");

			}else{
				WARN_S(pAddMe->name() << " missing parent");
			}
		}else{
			DBUG_LO("Blueprint '" << pAddMe->name() << "' added to library");
		}
	}
	++mScrBMapItr->second.mTimesAdded;
}

const cBlueprint*
cWorld::getBlueprint(dNameHash pNameHash){
	PROFILE;

	mScrBMapItr = mBlueprints.find(pNameHash);
	if(mScrBMapItr == mBlueprints.end())
		throw excep::notFound("couldn't find blueprint", __FILE__, __LINE__);

	return mScrBMapItr->second.mBlueprint;
}

void
cWorld::removeBlueprint(const cBlueprint* pRemoveMe){
	PROFILE;

	mScrBMapItr = mBlueprints.find(pRemoveMe->hash());

	if(mScrBMapItr == mBlueprints.end()){
		WARN_S("Tried to remove blueprint '" << pRemoveMe->mGetName() << "' when not drafted.");
		return;
	}

	--mScrBMapItr->second.mTimesAdded;
	if(mScrBMapItr->second.mTimesAdded > 0)
		return;

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

	//- If this figment replaced another, restore the original blueprint.
	if(pRemoveMe->replace() != uDoesntReplace){
		dBlueprintMap::iterator foundRep = mBlueprints.find(pRemoveMe->replace());
		dBlueprintMap::iterator foundArc = mBlueArchive.find(pRemoveMe->replace());
		if(foundRep != mBlueprints.end()){
			if(foundArc != mBlueArchive.end()){
				DBUG_LO("Blueprint '" << mScrBMapItr->second.mBlueprint->name() << "' restored. Archive entry removed"
				);
				foundRep->second = foundArc->second;
				mBlueArchive.erase(foundArc);

			}else{
				WARN_S("Replaced figment not found in archive.");
			}
		}else{
			WARN_S("Replaced figment not found in active blueprints.");
		}
	}

	mBlueprints.erase(mScrBMapItr);
	DBUG_LO("Erased blueprint: " << pRemoveMe->name());
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
	return makeFig(makeHash(PCStr2NStr(pName)));
}

ptrLead
cWorld::makeLead(cCommand::dUID pComID){
	PROFILE;
	ptrLead rtnLead(new cLead(pComID));
	return rtnLead;
}

ptrLead
cWorld::makeLead(const dPlaChar *aFigName, const dPlaChar *aComName){
	dNatStr totalString = PCStr2NStr(aFigName);
	totalString.t.append( PCStr2NStr(aComName).t );
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
cWorld::getPlugTag(const dPlaChar *figName, const dPlaChar *tagName){
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

	}
}

void
cWorld::primordial::cleanup(){
	//todo cleanup blueprints.
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

GTUT_START(test_world, drafting){
	tOutline<testDraftParent>::draft();
	tOutline<testDraftChild>::draft();
	GTUT_ASRT(gWorld.get()->makeFig(getHash<testDraftParent>())->hash() == getHash<testDraftParent>(), "didn't make the right figment");
	GTUT_ASRT(gWorld.get()->makeFig(getHash<testDraftChild>())->hash() == getHash<testDraftChild>(), "didn't make the right figment");

	tOutline<testDraftReplace>::draft();
	GTUT_ASRT(gWorld.get()->makeFig(getHash<testDraftReplace>())->hash() == getHash<testDraftReplace>(), "didn't make the right figment");
	GTUT_ASRT(gWorld.get()->makeFig(getHash<testDraftParent>())->hash() == getHash<testDraftReplace>(), "didn't make the right figment");
	GTUT_ASRT(gWorld.get()->makeFig(getHash<testDraftChild>())->hash() == getHash<testDraftChild>(), "didn't make the right figment");
}GTUT_END;

GTUT_START(test_world, extend_commands){
	gWorld.get()->getBlueprint(getHash<testDraftReplace>())->getCom(testDraftParent::xCommandA);
	gWorld.get()->getBlueprint(getHash<testDraftParent>())->getCom(testDraftParent::xCommandA);
	gWorld.get()->getBlueprint(getHash<testDraftChild>())->getCom(testDraftParent::xCommandA);

	cContext dontcare;
	ptrLead testLead = gWorld.get()->makeLead(testDraftParent::xCommandA);
	ptrFig replace = gWorld.get()->makeFig(getHash<testDraftReplace>());
	ptrFig parent = gWorld.get()->makeFig(getHash<testDraftParent>());
	ptrFig child = gWorld.get()->makeFig(getHash<testDraftChild>());

	replace->jack(testLead, &dontcare);
	parent->jack(testLead, &dontcare);
	child->jack(testLead, &dontcare);

}GTUT_END;


#endif

