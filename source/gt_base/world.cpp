#include "world.hpp"
#include "figment.hpp"

using namespace gt;

////////////////////////////////////////////////////////////
// Globals and statics
using namespace gt;

cWorld* 			gt::gWorld					=NULL;

cCoolStatic<cWorld::dLines> cWorld::xLines;
cCoolStatic<cProfiler> cWorld::xProfiler;

////////////////////////////////////////////////////////////
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
using namespace gt;

void
cWorld::lo(const dStr& pLine){
	PROFILE;

	xLines.get()->push_back(pLine);
}

cProfiler::cToken
cWorld::makeProfileToken(const dNatChar* pFile, unsigned int pLine){
	return xProfiler.get()->makeToken(pFile, pLine);
}

cWorld::cWorld():
	mKeepLooping(true)
{
	mLines = xLines.get();
	mProfiler = xProfiler.get();
	mVillageBicycle = ptrFig(new cEmptyFig());
	mBicycleSetup = false;
	mRoot = ptrFig(new cWorldShutoff());

	DBUG_VERBOSE_LO("World created.");
}

cWorld::~cWorld(){
	DBUG_VERBOSE_LO("It's the end of the world.");
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

	if(	// Some figments should never be remove.
		pRemoveMe->hash() == getHash<cFigment>()
		|| pRemoveMe->hash() == getHash<cEmptyFig>()
	)
		return;

	if(pRemoveMe->replace() != uDoesntReplace){
		mScrBMapItr = mBlueprints.find(pRemoveMe->replace());
		if(mScrBMapItr != mBlueprints.end()){
			mScrBMapItr = mBlueArchive.find(mScrBMapItr->first);
			if(mScrBMapItr != mBlueArchive.end()){
				mBlueprints[pRemoveMe->replace()] = mScrBMapItr->second;
			}
		}
	}

	mScrBMapItr = mBlueprints.find(pRemoveMe->hash());
	if(mScrBMapItr != mBlueprints.end()){
		std::list<ptrFig>*	branches = new std::list<ptrFig>();
		std::list<ptrFig>*	prev = new std::list<ptrFig>();
		std::map<cFigment*, ptrFig> figs;

		// Find and empty any objects using this blueprint.
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

		// empty the figments
		for(
			std::map<cFigment*, ptrFig>::iterator itr = figs.begin();
			itr != figs.end();
			++itr
		){
			DBUG_VERBOSE_LO( "Emptying figment " << reinterpret_cast<unsigned int>(itr->first) );
			itr->second = getEmptyFig();	// This should be enough to empty the figment.
		}

		DBUG_LO("Erasing blueprint " << pRemoveMe->hash());
		mBlueprints.erase(mScrBMapItr);
	}
}

ptrFig
cWorld::makeFig(dNameHash pNameHash){
	PROFILE;

	mScrBMapItr = mBlueprints.find(pNameHash);

	if(mScrBMapItr == mBlueprints.end())
		throw excep::base_error("bad name hash", __FILE__, __LINE__);

	return mScrBMapItr->second.mBlueprint->make();
}

void
cWorld::copyWorld(cWorld* pWorld){
	this->mLines->splice(this->mLines->end(), *pWorld->mLines);
	*this->mProfiler = *pWorld->mProfiler;
}

ptrLead
cWorld::makeLead(dNameHash pFigHash, dNameHash pComHash){
	PROFILE;

	mScrBMapItr =  mBlueprints.find(pFigHash);

	if(mScrBMapItr == mBlueprints.end())
		throw excep::base_error("bad name hash", __FILE__, __LINE__);

	return ptrLead(new cLead(mScrBMapItr->second.mBlueprint->getCom(pComHash)));
}

const cPlugTag* 
cWorld::getPlugTag(dNameHash pFigHash, dNameHash pPTHash){
	PROFILE;

	mScrBMapItr =  mBlueprints.find(pFigHash);

	if(mScrBMapItr == mBlueprints.end())
		throw excep::base_error("bad name hash", __FILE__, __LINE__);

	return mScrBMapItr->second.mBlueprint->getPlugTag(pPTHash);
}

void
cWorld::setRoot(ptrFig pNewRoot){
	mRoot = pNewRoot;
}

void
cWorld::makeProfileReport(std::ostream &log){
	mProfiler->flushThatLog(log);
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

////////////////////////////////////////////////////////////
void
gt::redirectWorld(cWorld* pWorldNew){
	gt::gWorld = new gt::cWorld();	// This should give you a reference to copy the statics
	pWorldNew->copyWorld(gt::gWorld);
	delete gt::gWorld;

	gt::cWorld::xLines.set(pWorldNew->mLines);
	gt::cWorld::xProfiler.set(pWorldNew->mProfiler);
	gt::gWorld = pWorldNew;
}

////////////////////////////////////////////////////////////
// Tests
#ifdef GTUT



#endif
