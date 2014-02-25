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
// World

using namespace gt;

cWorld::cWorld():
	mKeepLooping(true)
{
	//- Assume these figments will have their blueprints managed properly later.
	mVillageBicycle = ptrFig(new cEmptyFig());
	mRoot = ptrFig(new cWorldShutoff());

	(void)primordial::makeProfileToken(__FILE__, __LINE__); //- Ensure it exists.
	mProfiler = primordial::xProfiler;

	primordial::lo("World created."); //- Ensure lines exists.
	mLines = primordial::xLines;

#	ifdef GT_THREADS
		mProfileGuard = primordial::xProfileGuard;
		mLineGuard = primordial::xLineGuard;
#	endif
}

cWorld::~cWorld(){
}

void
cWorld::closeWorld(){
	mRoot.redirect(NULL);
	mVillageBicycle.redirect(NULL);
	dAddons::iterator found;

	while(!mOpenAddons.empty()){
		dAddons::iterator found = mAvailableAddons.find(*mOpenAddons.begin());
		if(found == mAvailableAddons.end()){
			WARN_S("open addon not found in available addons");
			mOpenAddons.erase(mOpenAddons.begin());
			continue;
		}

		closeAddon(found->second);
	}

	for(dBlueprintMap::iterator bp = mBlueprints.begin(); bp != mBlueprints.end(); ++bp){
		if(bp->second.mUsing != NULL)
			bp->second.mUsing->mCleanup();
	}

	mBlueprints.clear();
}

void
cWorld::addBlueprint(cBlueprint* pAddMe, const dStr &pFromAddon){
	ASRT_NOTNULL(pAddMe);

	DBUG_LO("Adding blueprint " << pAddMe->name());

	dNameHash addon = 0;
	if(!pFromAddon.empty()){
		addon = makeHash(pFromAddon.c_str());
		if(mAvailableAddons.find(addon) == mAvailableAddons.end())
			THROW_ERROR(pAddMe->name() << " required addon " << pFromAddon << " not supported.");
	}

	dBlueprintMap::iterator bp = mBlueprints.find(pAddMe->hash());

	if(bp == mBlueprints.end()){
		bp = mBlueprints.insert(
			mBlueprints.begin(),
			dBlueprintMap::value_type(pAddMe->hash(), sBlueBook())
		);
	}

	if(addon != 0){
		bp->second.mOpenedWith = addon;
		bp->second.mInAddons.insert(addon);
	}

	if(pAddMe->replace() != uDoesntReplace){
		dBlueprintMap::iterator replace = mBlueprints.find(pAddMe->replace());
		if(replace != mBlueprints.end()){
			if(replace->second.mUsing != NULL){
				if(replace->second.mArchived == NULL){
					replace->second.mArchived = replace->second.mUsing;
					replace->second.mArchiveOW = replace->second.mOpenedWith;
					replace->second.mOpenedWith = addon;
					replace->second.mUsing = pAddMe;
					DBUG_VERBOSE_LO("	" << replace->second.mArchived->name() << " replaced by " << pAddMe->name());
				}else{
					DBUG_VERBOSE_LO("	" << replace->second.mArchived->name() << " already replaced.");
				}
			}else{
				DBUG_VERBOSE_LO("	" << pAddMe->name() << " can't replace inactive blueprint.");
			}
		}else{
			WARN_S("	" << pAddMe->name() << " couldn't find replacement blueprint");
		}
	}

	bp->second.mUsing = pAddMe;
}

const cBlueprint*
cWorld::getBlueprint(dNameHash pNameHash){
	PROFILE;

	dBlueprintMap::iterator found = mBlueprints.find(pNameHash);
	if(found == mBlueprints.end())
		return NULL;

	return found->second.mUsing;
}

cWorld::dBlueList
cWorld::getAllBlueprints(){
	dBlueList out;

	for(dBlueprintMap::iterator itr = mBlueprints.begin(); itr != mBlueprints.end(); ++itr){
		if(itr->second.mUsing != NULL){
			if(itr->second.mArchived != NULL)
				out.push_back(itr->second.mArchived);
			else
				out.push_back(itr->second.mUsing);
		}
	}

	return out;
}

void
cWorld::removeBlueprint(const cBlueprint* pRemoveMe, const dStr &pFromAddon){
	PROFILE;

	DBUG_LO("Removing blueprint " << pRemoveMe->name());

	dBlueprintMap::iterator found = mBlueprints.find(pRemoveMe->hash());
	if(found == mBlueprints.end()){
		WARN_S("	Tried to remove a blueprint '" << pRemoveMe->mGetName() << "' that didn't exist.");
		return;
	}

	if(found->second.mUsing == NULL){
		WARN_S("	Tried to remove an inactive blueprint " << pRemoveMe->mGetName());
		return;
	}

	//- Find and empty any objects using this blueprint.
	std::list<ptrFig> figs;
	findFigs(pRemoveMe->hash(), &figs);

	for(std::list<ptrFig>::iterator itr = figs.begin(); itr != figs.end(); ++itr){
		DBUG_VERBOSE_LO( "	Emptying figment " << (*itr)->name() );
		itr->redirect(getEmptyFig());
	}

	//- Make blueprint inactive or restore from archive.
	if(found->second.mArchived == NULL){
		dBlueprintMap::iterator replaced = mBlueprints.find(
			found->second.mUsing->replace()
		);
		if(replaced != mBlueprints.end()){
			replaced->second.mUsing = replaced->second.mArchived;
			replaced->second.mArchived = NULL;
		}
		found->second.mUsing = NULL;
		found->second.mOpenedWith = 0;

	}else{
		found->second.mUsing = found->second.mArchived;
		found->second.mOpenedWith = found->second.mArchiveOW;
		found->second.mArchived = NULL;
		found->second.mArchiveOW = 0;
	}

}

ptrFig
cWorld::makeFig(dNameHash pNameHash){
	PROFILE;

	dBlueprintMap::iterator found = mBlueprints.find(pNameHash);
	if(found == mBlueprints.end())
		throw excep::notFound("Blueprint", __FILE__, __LINE__);

	if(found->second.mUsing == NULL){
		if(found->second.mInAddons.empty())
			THROW_ERROR(pNameHash << " no addons have this blueprint.");

		dAddons::iterator addon = mAvailableAddons.find( *found->second.mInAddons.begin() );
		if(addon == mAvailableAddons.end())
			THROW_ERROR("bad addon hash");

		openAddon(addon->second);

		if(found->second.mUsing == NULL)
			THROW_ERROR(pNameHash << " could not be made by addon " << addon->second);
	}

	return found->second.mUsing->make();
	return ptrFig(NULL);
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

const cPlugTag* 
cWorld::getPlugTag(dNameHash pFigBlueprint, cPlugTag::dUID pPTHash){
	PROFILE;

	dBlueprintMap::iterator found =  mBlueprints.find(pFigBlueprint);
	if(found == mBlueprints.end())
		THROW_ERROR("blueprint wasn't found");

	if(found->second.mUsing == NULL)
		THROW_ERROR("blueprint is inactive");

	return found->second.mUsing->getPlugTag(pPTHash);
}

const cPlugTag*
cWorld::getPlugTag(const dPlaChar *figName, const dPlaChar *tagName){
	return getPlugTag(
		makeHash(toNStr(figName)),
		makeHash(toNStr(tagName))
	);
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

void
cWorld::checkAddons(){
	dAddons addons;
	dAddons::iterator addName;
	dBlue2Addons bluesInAddons;
	dAddon2Fresh fresh;
	bool writeCache=false;

	getAddonList(addons);
	mAvailableAddons = addons;
	readAddonCache(addons, bluesInAddons, fresh);
	for(dAddon2Fresh::iterator f = fresh.begin(); f != fresh.end(); ++f){
		addName = addons.find(f->first);
		if(!f->second && addName != addons.end()){
			writeCache = true;

			tAutoPtr<cWorld> underworld(makeWorld());
			underworld->mAvailableAddons = addons;
			underworld->openAddon(addName->second);
			dBlueList blueprints = underworld->getAllBlueprints();
			dBlue2Addons::iterator existing;
			for(dBlueList::iterator b = blueprints.begin(); b != blueprints.end(); ++b){
				existing = bluesInAddons.find((*b)->name());
				if(existing == bluesInAddons.end())
					existing = bluesInAddons.insert(
						bluesInAddons.begin(),
						dBlue2Addons::value_type((*b)->name(), dRefAddons())
					);

				existing->second.insert(addName->first);
			}
			underworld->closeAddon(addName->second);
		}
	}

	if(writeCache)
		writeAddonCache(bluesInAddons);

	{
		dNameHash bname;
		dBlueprintMap::iterator found;
		for(dBlue2Addons::iterator b = bluesInAddons.begin(); b != bluesInAddons.end(); ++b){
			bname = makeHash(PCStr2NStr(b->first.c_str()));
			found = mBlueprints.find(bname);
			if(found == mBlueprints.end())
				found = mBlueprints.insert(mBlueprints.begin(), dBlueprintMap::value_type(bname, sBlueBook()));

			found->second.mInAddons = b->second;
		}
	}
}

void
cWorld::findFigs(dNameHash pName, std::list<ptrFig> *output){
	if(!mRoot.valid())
		return;

	if(mRoot.get() == NULL)
		return;

	std::set<iFigment*> scanned;
	std::list<ptrFig> linksA, linksB;
	std::list<ptrFig>* branches = &linksA;
	std::list<ptrFig>* prev = &linksB;

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
			if(scanned.find(i->get()) != scanned.end())
				continue;

			scanned.insert(i->get());
			(*i)->getLinks( branches );	//add to the next series of branches.

			if(i->get()->hash() == pName)
				output->push_back(*i);
		}

	}while(branches->size() > 0);
}

void closeUselessAddons(){

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
	ss << "Warning detected in file '" << pFile << "' on line " << pLine << std::endl << ". Info: " << msg;
	lo(ss.str());
}

void
cWorld::primordial::warnError(std::exception &pE, const char* pFile, const unsigned int pLine){
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
		lo("");	//- This shouldn't add a line, but it ensures it's initialised.
		(void)makeProfileToken(NULL, 0);	//- same.

		{
#			ifdef GT_THREADS
				boost::lock_guard<boost::recursive_mutex> lockA(*xLineGuard);
				boost::lock_guard<boost::recursive_mutex> lockB(*xProfileGuard);
				boost::lock_guard<boost::recursive_mutex> lockC(*pWorldNew->mProfileGuard);
				boost::lock_guard<boost::recursive_mutex> lockD(*pWorldNew->mLineGuard);
#			endif

			if(!xLines->empty())
				pWorldNew->mLines->splice(pWorldNew->mLines->end(), *xLines);

			*pWorldNew->mProfiler += *xProfiler;

			delete xLines;
			delete xProfiler;

			xLines = pWorldNew->mLines;
			xProfiler = pWorldNew->mProfiler;
		}

#		ifdef GT_THREADS
			delete xProfileGuard;
			delete xLineGuard;

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

