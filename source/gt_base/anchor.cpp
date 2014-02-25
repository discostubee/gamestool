
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

cAnchor::cAnchor(){
}

cAnchor::~cAnchor() {
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

	if(!mLink.get().valid())
		return;

	try{
		mLink.get()->getLinks(branches);
		figs.insert( mLink.get().get() );

		//todo Must prevent circular references.
		do{ //- while there are branches still left to explore.
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

		DBUG_VERBOSE_LO("anchor saving " << figs.size() << " figments" );

		chunkSig = reinterpret_cast<dFigSaveSig>(mLink.get().get());
		pAddHere->add( &chunkSig ); // Save reference to the root.

		for( std::set<iFigment*>::iterator i = figs.begin(); i != figs.end(); ++i ){	//- Now save each figment.
			PROFILE;
			cByteBuffer	chunkSave;

			//- The process below must happen in exactly the same way when loading.
			//- We are counting the hash and reload sig as part of the chunk size as a kind of check against corruption.
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

	DUMB_REF_PAR(pReloads);

	dReloadMap		reloads;
	dFigSaveSig		rootSig;
	dNameHash		tempHash;
	dFigSaveSig		reloadSig;
	size_t			readSpot=0, chunkStart, chunkSize;

	ASRT_NOTNULL(pBuff);

	try{	//- The process below must happen in exactly the same way in the save process.

		//- the root sig.
		readSpot += pBuff->fill(&rootSig, readSpot);

		//- Now lets read in all the chunks.
		while(pBuff->size() > readSpot){
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
			}else if(chunkSize == 0){	//- No additional data apart from the figment itself.
				reloads[reloadSig] = new cReload( ptrFig(gWorld.get()->makeFig(tempHash)) );
			}else{
				WARN_S("Unable to load chunk: bad size");
			}
		}

		//- Now re-load all the figs we've made. It's done on a separate loop to the one above so that figment references can be re-created.
		for(dReloadMap::iterator itr = reloads.begin(); itr != reloads.end(); ++itr){
			DBUG_VERBOSE_LO("		loading " << itr->second->fig->name());
			try{
				itr->second->fig->loadEat( &itr->second->data, &reloads );
			}catch(excep::base_error &e){
				WARN(e);
			}
		}

		mLink.get() = reloads[rootSig]->fig;

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


////////////////////////////////////////////////////////////

#ifdef GTUT

#include "runList.hpp"
#include "unitTestFigments.hpp"

tPlug<ptrBuff> plugBuff;
const dPlaChar *testStr = "proper job";

GTUT_START(testAnchor, basicSave){
	tOutline<cSaveTester>::draft();

	plugBuff = ptrBuff(new cByteBuffer());

	cContext fakeCon;
	ptrFig ank = gWorld.get()->makeFig(getHash<cAnchor>());

	tPlug<ptrFig> tester;
	tester = ptrFig(new cSaveTester(testStr, 42));

	ptrLead add = gWorld.get()->makeLead(cAnchor::xSetLink);
	add->linkPlug(&tester, cAnchor::xPT_links);

	ank->jack(add, &fakeCon);
	ank->save(plugBuff.get().get());
}GTUT_END;

GTUT_START(testAnchor, basicLoad){
	ptrFig ank = gWorld.get()->makeFig(getHash<cAnchor>());
	cContext fake;

	{
		ptrLead load = gWorld.get()->makeLead(cAnchor::xLoad);
		dReloadMap dontcare;
		ank->loadEat(plugBuff.get().get(), &dontcare);
	}

	tPlug<ptrFig> tester;
	{
		ptrLead getlink = gWorld.get()->makeLead(cAnchor::xGetLinks);

		getlink->linkPlug(&tester, cAnchor::xPT_links);
		ank->jack(getlink, &fake);
	}

	tPlug<dStr> myStr("");
	tPlug<int> myNum(0);
	ptrLead checkData = gWorld.get()->makeLead(cSaveTester::xGetData);

	PLUG_REFRESH(tester);
	tester.get()->jack(checkData, &fake);

	startLead(checkData, fake.getSig());
	GTUT_ASRT(checkData->copyPlug(&myStr, cSaveTester::xPT_str), "didn't find plug");
	GTUT_ASRT(checkData->copyPlug(&myNum, cSaveTester::xPT_num), "didn't find plug");
	stopLead(checkData);

	PLUG_REFRESH(myStr);
	PLUG_REFRESH(myNum);
	GTUT_ASRT(myNum.get()==42, "saved numbers are not the same");
	GTUT_ASRT(myStr.get().compare(testStr)==0, "saved string doesn't match");

	plugBuff.get().reset();

}GTUT_END;

GTUT_START(testAnchor, chainSave){
	cContext fakeCon;
	ptrFig ank = gWorld.get()->makeFig(getHash<cAnchor>());
	tPlug<ptrFig> rlist = gWorld.get()->makeFig(getHash<cRunList>());

	ptrLead linkRoot = gWorld.get()->makeLead(cAnchor::xSetLink);
	linkRoot->linkPlug(&rlist, cAnchor::xPT_links);
	ank->jack(linkRoot, &fakeCon);

	tPlug<ptrFig> tester = gWorld.get()->makeFig(getHash<cSaveTester>());
	ptrLead add = gWorld.get()->makeLead(cRunList::xAdd);
	add->linkPlug(&tester, cRunList::xPT_links);
	rlist.get()->jack(add, &fakeCon);

	ptrLead save = gWorld.get()->makeLead(cAnchor::xSave);
	plugBuff = ptrBuff(new cByteBuffer());
	save->linkPlug(&plugBuff, cAnchor::xPT_serialBuff);
	ank->jack(save, &fakeCon);

}GTUT_END;

GTUT_START(testAnchor, chainLoad){
	cContext fakeCon;
	ptrFig ank = gWorld.get()->makeFig(getHash<cAnchor>());
	dReloadMap dontcare;

	ank->loadEat(plugBuff.get().get(), &dontcare);

	std::list<ptrFig> ankLinks;
	ank->getLinks(&ankLinks);
	if(ankLinks.empty() || strcmp( ankLinks.front()->name(), cRunList::identify() )!=0 )
		GTUT_ASRT(false, "run list wasn't the root of anchor");

	std::list<ptrFig> runLinks;
	ankLinks.front()->getLinks(&runLinks);

	if(runLinks.empty() || strcmp( runLinks.front()->name(), cSaveTester::identify() )!=0 )
		GTUT_ASRT(false, "run list doesn't have the save tester");

	GTUT_ASRT(
		strncmp(
			runLinks.front()->name(),
			cSaveTester::identify(),
			strlen( cSaveTester::identify())
		)==0,
		"didn't save/load correct figment"
	);

	plugBuff.get().reset();
}GTUT_END;

GTUT_START(test_cAnchor, test_suit){
	figmentTestSuit<cAnchor>();
}GTUT_END;

#endif
