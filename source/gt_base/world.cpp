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

bool cWorld::thereCanBeOnlyOne = false;

////////////////////////////////////////////////////////////
// Blueprint stuff
using namespace gt;

//!\brief	Used to keep track of the things which may have been replaced by this blueprint.
struct cWorld::sBlueprintHeader{
	const cBlueprint* mBlueprint;
	dNameHash mReplaced;

	sBlueprintHeader():
		mReplaced(uDoesntReplace)
	{}

	sBlueprintHeader(const cBlueprint* pBlue, dNameHash pName):
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
	PROFILE;

	static bool linesSetup = false;
	if(!linesSetup){
		linesSetup = true;
		xLines = new dLines();
	}
	xLines->push_back(pLine);
}

cProfiler::cToken
cWorld::makeProfileToken(const dNatChar* pFile, unsigned int pLine){
	static bool profileSetup = false;
	if(!profileSetup){
		profileSetup = true;
		xProfiler = new cProfiler();
	}
	return xProfiler->makeToken(pFile, pLine);
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

	makeProfileToken("", 0); //- Ensure it exists.
	mProfiles = xProfiler;

	lo("World created."); //- Ensure it exists.
	mLines = xLines;
}

cWorld::~cWorld(){
	makeProfileToken("", 0); //- Ensure it exists.

	delete xProfiler;

	lo(""); //- Ensure it exists.
	flushLines();
	delete xLines;
}

void
cWorld::addBlueprint(const cBlueprint* pAddMe){
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
}

ptrLead
cWorld::makeLead(dNameHash pFigHash, dNameHash pComHash, dConSig pConx){
	PROFILE;

	mScrBMapItr =  mBlueprints.find(pFigHash);

	if(mScrBMapItr == mBlueprints.end())
		throw excep::base_error("bad name hash", __FILE__, __LINE__);

	return ptrLead(new cLead( mScrBMapItr->second.mBlueprint->getCom(pComHash), pConx ));
}

const cPlugTag* 
cWorld::gecPlugTag(dNameHash pFigHash, dNameHash pPTHash){
	PROFILE;

	mScrBMapItr =  mBlueprints.find(pFigHash);

	if(mScrBMapItr == mBlueprints.end())
		throw excep::base_error("bad name hash", __FILE__, __LINE__);

	return mScrBMapItr->second.mBlueprint->gecPlugTag(pPTHash);
}

void
cWorld::setRoot(ptrFig pNewRoot){
	mRoot = pNewRoot;
}

void
cWorld::makeProfileReport(std::ostream &log){

}

void
cWorld::warnError(excep::base_error &pE, const dNatChar* pFile, const unsigned int pLine){
	std::stringstream ss;
	ss << "!Warning detected in file " << pFile << " on line " << pLine << std::endl << "	" << pE.what();
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
		gWorld.cleanup();
	}

}
