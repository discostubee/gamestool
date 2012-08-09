
/**********************************************************************************************************
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
 *********************************************************************************************************/

#include "anchor.hpp"

////////////////////////////////////////////////////////////
using namespace gt;

const cPlugTag* cAnchor::xPT_root = tOutline<cAnchor>::makePlugTag("root");

const cCommand::dUID cAnchor::xSetRoot = tOutline<cAnchor>::makeCommand(
	"set root",
	&cAnchor::patSetRoot,
	cAnchor::xPT_root,
	NULL
);

const cCommand::dUID cAnchor::xGetRoot = tOutline<cAnchor>::makeCommand(
	"get root",
	&cAnchor::patGetRoot,
	cAnchor::xPT_root,
	NULL
);

void
cAnchor::work(cContext* pCon) {
	mRoot.get()->run(pCon);
}

void
cAnchor::save(cByteBuffer* pAddHere){
	PROFILE;

	ASRT_NOTNULL(pAddHere);

	std::list<ptrFig>*	branches = new std::list<ptrFig>();
	std::list<ptrFig>*	prev = new std::list<ptrFig>();
	std::set<iFigment*> figs;

	size_t				chunkSize = 0;
	dNameHash			chunkHash = 0;
	dFigSaveSig			chunkSig = 0;

	try{
		mRoot.get()->getLinks(branches);
		figs.insert( mRoot.get().get() );

		do{ //- while there are branches still left to explore. Must prevent circular references.
			PROFILE;
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
					figs.insert( i->get() );
					(*i)->getLinks( branches );	//add to the next series of branches.
				}
			}
		}while(branches->size() > 0);

		DBUG_VERBOSE_LO(":) anchor saving " << figs.size() << " figments" );

		{	//- First, we must list any required figments
			std::list<dStr> addons;
			for(std::set<iFigment*>::iterator i = figs.begin(); i != figs.end(); ++i){
				if(!(*i)->requiredAddon().empty())
					addons.push_back( (*i)->requiredAddon() );
			}

			size_t numAddons = addons.size();
			pAddHere->add(&numAddons);

			while(!addons.empty()){
				pAddHere->add(&addons.front());
				addons.pop_front();
			}
		}

		chunkSig = reinterpret_cast<dFigSaveSig>(mRoot.get().get());
		pAddHere->add( &chunkSig ); // Save reference to the root.

		for( std::set<iFigment*>::iterator i = figs.begin(); i != figs.end(); ++i ){	//- Now save each figment.
			PROFILE;
			cByteBuffer	chunkSave;

			//- The process below must happen in exactly the same way when loading.
			//- We are counting the hash as part of the chunk size as a kind of check against corruption.
			try{
				DBUG_VERBOSE_LO("	saving a " << (*i)->name());

				chunkHash = (*i)->getReplacement();	//- Get the hash for this figment's parent, not its own. That way, when we reload this file the native replacements are used.
				if( chunkHash == uDoesntReplace ){
					chunkHash = (*i)->hash();
				}
				chunkSave.add(&chunkHash);

				chunkSig = reinterpret_cast<dFigSaveSig>(*i);	//- use the pointer as a way to identify all the different figments in the tree.
				chunkSave.add(&chunkSig);

				(*i)->save(&chunkSave);

				chunkSize = chunkSave.size();
				pAddHere->add(&chunkSize);

				pAddHere->add(chunkSave);

			}catch(excep::base_error &e){
				WARN(e);
			}
		}

	}catch(...){
		delete branches;
		delete prev;

		throw;
	}

	delete branches;
	delete prev;
}

void
cAnchor::loadEat(cByteBuffer* pBuff, dReloadMap* pReloads){
	PROFILE;

	dReloadMap		reloads;
	dFigSaveSig		rootSig;
	dNameHash		tempHash;
	dFigSaveSig		reloadSig;
	size_t			readSpot=0;
	int 			chunkStart, chunkSize;

	static const size_t BLOCK_SIZE = sizeof(size_t) + sizeof(dNameHash) + sizeof(dFigSaveSig);

	ASRT_NOTNULL(pBuff);

	try{	//- The process below must happen in exactly the same way in the save process.
		DBUG_VERBOSE_LO(":) anchor loading");

		//- first lets get all the addons we need to load these figments.
		size_t numAddons = 0;
		readSpot += pBuff->fill(&numAddons, readSpot);

		DBUG_VERBOSE_LO("Number of addons: " << numAddons)

		for(size_t i=0; i<numAddons; ++i){
			dStr addonName;
			readSpot += pBuff->fill(&addonName, readSpot);
			gWorld.get()->openAddon(addonName);
		}

		//- now get the root sig.
		readSpot += pBuff->fill(&rootSig,readSpot);

		//- Now lets read in all the chunks.
		while(readSpot + BLOCK_SIZE <= pBuff->size()){
			readSpot += pBuff->fill(&chunkSize, readSpot); //- The size of the actual chunk size value is not counted.

			chunkStart = readSpot;
			readSpot += pBuff->fill(&tempHash, readSpot);
			readSpot += pBuff->fill(&reloadSig, readSpot); //- next up we take the signature of this figment which will be used when figments reference each other.

			chunkSize -= (readSpot - chunkStart);

			if(chunkSize>0){
				reloads[reloadSig] = new cReload(
					ptrFig(gWorld.get()->makeFig(tempHash)),
					pBuff->get(readSpot),
					chunkSize
				);
				readSpot += chunkSize;
			}else if(chunkSize == 0){	//- No additional data appart from the figment itself.
				reloads[reloadSig] = new cReload( ptrFig(gWorld.get()->makeFig(tempHash)) );
			}else{
				WARN_S("Unable to load chunk: bad size");
			}
		}

		//- Now re-load all the figs we've made. It's done on a separate loop to the one above so that figment references can be re-created.
		for(dReloadMap::iterator itr = reloads.begin(); itr != reloads.end(); ++itr){
			DBUG_LO("		loading " << itr->second->fig->name());
			try{
				itr->second->fig->loadEat( &itr->second->data, &reloads );
			}catch(excep::base_error &e){
				WARN(e);
			}
		}

		mRoot.get() = reloads[rootSig]->fig;

	}catch(...){
		for(dReloadMap::iterator itr = reloads.begin(); itr != reloads.end(); ++itr){	//- Cleanup
			SAFEDEL(itr->second);
		}
		throw;
	}

	for(dReloadMap::iterator itr = reloads.begin(); itr != reloads.end(); ++itr){	//- Cleanup
		SAFEDEL(itr->second);
	}
	pBuff->trimHead(readSpot);
}

cAnchor::cAnchor(){
	addUpdRoster(&mRoot);
}

cAnchor::~cAnchor() {
}

void
cAnchor::patSetRoot(ptrLead aLead){
	aLead->getPlug(&mRoot, cAnchor::xPT_root);
}

void
cAnchor::patGetRoot(ptrLead aLead){
	aLead->addPlug(&mRoot, cAnchor::xPT_root);
}

void
cAnchor::getLinks(std::list<ptrFig>* pOutLinks){
	pOutLinks->push_back(mRoot.get());
}

////////////////////////////////////////////////////////////

#ifdef GTUT

GTUT_START(test_cAnchor, test_suit){
	tOutline<cFigment>::draft();
	figmentTestSuit<cAnchor>();
}GTUT_END;

#include "runList.hpp"

class cSaveTester: public cFigment, private tOutline<cSaveTester>{
public:
	static const cPlugTag *xPT_str, *xPT_num;
	static const cCommand::dUID	xGetData;

	cSaveTester(){}
	cSaveTester(const dPlaChar* inStr, int pNum) : myStr(inStr), myNum(pNum) {}
	virtual ~cSaveTester(){}

	static const dPlaChar* identify(){ return "save tester"; }
	virtual const dPlaChar* name() const{ return cSaveTester::identify(); }
	virtual dNameHash hash() const{ return getHash<cSaveTester>(); }

	virtual void save(cByteBuffer* pAddHere) {
		myStr.save(pAddHere); myNum.save(pAddHere);
	}

	virtual void loadEat(cByteBuffer* pBuff, dReloadMap* pReloads = NULL) {
		myStr.loadEat(pBuff, pReloads); myNum.loadEat(pBuff, pReloads);
	}
private:
	tPlug<dStr> myStr;
	tPlug<int> myNum;

	void patGetData(ptrLead aLead);
};

const cPlugTag *cSaveTester::xPT_str = tOutline<cSaveTester>::makePlugTag("my str");

const cPlugTag *cSaveTester::xPT_num = tOutline<cSaveTester>::makePlugTag("my num");

const cCommand::dUID	cSaveTester::xGetData = tOutline<cSaveTester>::makeCommand(
	"get my string", &cSaveTester::patGetData,
	xPT_str,
	xPT_num,
	NULL
);

void
cSaveTester::patGetData(ptrLead aLead){
	aLead->addPlug(&myStr, xPT_str);
	aLead->addPlug(&myNum, xPT_num);
}

tPlug<iFigment::ptrBuff> plugBuff;
//const dTextChar *testStr = L"proper job";
const dPlaChar *testStr = "proper job";

GTUT_START(testAnchor, basicSave){
	tOutline<cFigment>::draft();
	tOutline<cSaveTester>::draft();
	tOutline<cAnchor>::draft();
	cContext fakeCon;
	tPlug<ptrFig> tester;
	ptrFig ank = gWorld.get()->makeFig(getHash<cAnchor>());

	plugBuff = iFigment::ptrBuff( iFigment::ptrBuff(new cByteBuffer()) );
	tester = ptrFig(new cSaveTester(testStr, 42));

	ptrLead add = gWorld.get()->makeLead(cAnchor::xSetRoot);
	add->addPlug(&tester, cAnchor::xPT_root);

	ank->jack(add, &fakeCon);
	ank->save(plugBuff.get().get());
}GTUT_END;

GTUT_START(testAnchor, basicLoad){
	ptrFig ank = gWorld.get()->makeFig(getHash<cAnchor>());
	cContext fake;

	ptrLead load = gWorld.get()->makeLead(cAnchor::xLoad);
	dReloadMap dontcare;
	ank->loadEat(plugBuff.get().get(), &dontcare);

	ptrLead root = gWorld.get()->makeLead(cAnchor::xGetRoot);
	ank->jack(root, &fake);
	tPlug<ptrFig> reload;

	startLead(root, fake.getSig());
	root->getPlug(&reload, cAnchor::xPT_root);
	stopLead(root);

	ptrLead checkData = gWorld.get()->makeLead(cSaveTester::xGetData);
	reload.get()->jack(checkData, &fake);

	tPlug<dStr> myStr;
	tPlug<int> myNum;

	startLead(checkData, fake.getSig());
	checkData->getPlug(&myStr, cSaveTester::xPT_str);
	checkData->getPlug(&myNum, cSaveTester::xPT_num);
	stopLead(checkData);

	GTUT_ASRT(myNum.get()==42, "saved numbers are not the same");
	GTUT_ASRT(myStr.get().compare(testStr)==0, "saved string doesn't match");

	plugBuff.get().reset();
}GTUT_END;


GTUT_START(testAnchor, figmentSave){
	tOutline<cFigment>::draft();
	tOutline<cSaveTester>::draft();
	tOutline<cAnchor>::draft();
	tOutline<cRunList>::draft();
	cContext fakeCon;
	ptrFig ank = gWorld.get()->makeFig(getHash<cAnchor>());
	tPlug<ptrFig> rlist = gWorld.get()->makeFig(getHash<cRunList>());

	ptrLead addRoot = gWorld.get()->makeLead(cAnchor::xSetRoot);
	addRoot->addPlug(&rlist, cAnchor::xPT_root);
	ank->jack(addRoot, &fakeCon);

	tPlug<ptrFig> tester = gWorld.get()->makeFig(getHash<cSaveTester>());
	ptrLead add = gWorld.get()->makeLead(cRunList::xAdd);
	add->addToPile(&tester);
	rlist.get()->jack(add, &fakeCon);

	ptrLead save = gWorld.get()->makeLead(cAnchor::xSave);
	plugBuff = iFigment::ptrBuff( iFigment::ptrBuff(new cByteBuffer()) );
	save->addPlug(&plugBuff, cAnchor::xPT_serialBuff);
	ank->jack(save, &fakeCon);

}GTUT_END;

GTUT_START(testAnchor, figmentLoad){
	tOutline<cSaveTester>::draft();
	tOutline<cAnchor>::draft();
	tOutline<cRunList>::draft();
	cContext fakeCon;
	ptrFig ank = gWorld.get()->makeFig(getHash<cAnchor>());
	dReloadMap dontcare;

	ank->loadEat(plugBuff.get().get(), &dontcare);

	std::list<ptrFig> links;
	ank->getLinks(&links);
	if(links.empty())
		GTUT_ASRT(false, "root of anchor not in links list");

	links.front()->getLinks(&links);

	if(links.size() != 2)
		GTUT_ASRT(false, "tester not in links list");

	links.pop_front();

	GTUT_ASRT(strncmp( links.front()->name(), cSaveTester::identify(), strlen( cSaveTester::identify()) )==0, "didn't save/load correct figment");

	plugBuff.get().reset();
}GTUT_END;

#endif
