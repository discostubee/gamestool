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
cAnchor::save(cByteBuffer* pAddHere){
	PROFILE;

	std::list<ptrFig>*	branches = new std::list<ptrFig>();
	std::list<ptrFig>*	prev = new std::list<ptrFig>();
	std::set<iFigment*> figs;

	size_t				chunkSize = 0;
	dNameHash			chunkHash = 0;
	tPlug<size_t>		currentSpot;
	dFigSaveSig			chunkSig = 0;

	currentSpot.mD = 0;

	try{
		mRoot.mD->getLinks(branches);
		figs.insert( mRoot.mD.get() );

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

		DBUG_LO(":) anchor saving " << figs.size() << " figments" );

		chunkSig = reinterpret_cast<dFigSaveSig>(mRoot.mD.get());
		pAddHere->add( &chunkSig ); // Save reference to the root.

		for( std::set<iFigment*>::iterator i = figs.begin(); i != figs.end(); ++i ){
			PROFILE;
			cByteBuffer	chunkSave;

			//- Get the hash for this figment's parent, not its own. That way, when we reload this file the native replacements are used.
			chunkHash = (*i)->getReplacement();
			if( chunkHash == uDoesntReplace ){
				chunkHash = (*i)->hash();
			}

			//- Get additional save data.
			try{
				 (*i)->save(&chunkSave);
			}catch(excep::dontUseThis){
			}

			chunkSize = sizeof(dNameHash) + sizeof(dFigSaveSig) + chunkSave.size(); // We are counting the hash as part of the chunk size as a kind of check against corruption.

			DBUG_LO("	saving a " << (*i)->name());

			chunkSig = reinterpret_cast<dFigSaveSig>(*i);

			//- Add it to the buffer. The process below must happen in exactly the same way when loading.
			pAddHere->add( &chunkSize );
			pAddHere->add( &chunkHash );
			pAddHere->add( &chunkSig );			//- use the pointer as a way to identify all the different figments in the tree.
			if(chunkSave.size() > 0)
				pAddHere->add(chunkSave);
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
	size_t			chunkSize;
	dNameHash		tempHash;
	dFigSaveSig		reloadSig;
	size_t			readSpot=0;

	static const size_t BLOCK_SIZE = sizeof(size_t) + sizeof(dNameHash) + sizeof(dFigSaveSig);

	ASRT_NOTNULL(pBuff);

	try{
		DBUG_LO(":) anchor loading");

		//- The process below must happen in exactly the same way in the save process.
		pBuff->fill(&rootSig); //- the first entry in the file is the root signature.
		readSpot += sizeof(rootSig);

		//- Now lets read in all the chunks.
		while(readSpot + BLOCK_SIZE <= pBuff->size()){
			pBuff->fill(&chunkSize, readSpot); //- First, let's get the size of this entire chunk.
			readSpot += sizeof(chunkSize);

			pBuff->fill(&tempHash, readSpot);	//- now, let's get the name hash needed to spawn the figment.
			readSpot += sizeof(tempHash);

			pBuff->fill(&reloadSig, readSpot); //- next up we take the signature of this figment which will be used when figments reference each other.
			readSpot += sizeof(reloadSig);

			chunkSize -= (sizeof(tempHash)+sizeof(reloadSig));

			if(chunkSize>0){	//- lastly, lets make the figment and store its additional data for later.
				reloads[reloadSig] = new cReload(
					ptrFig(gWorld.get()->makeFig(tempHash)),
					pBuff->get(readSpot),
					chunkSize
				);
				readSpot += chunkSize;
			}else{	//- No additional data appart from the figment itself.
				reloads[reloadSig] = new cReload( ptrFig(gWorld.get()->makeFig(tempHash)) );
			}
		}

		//- Now re-load all the figs we've made. It's done on a separate loop to the on above so that figment references can be re-created.
		for(dReloadMap::iterator itr = reloads.begin(); itr != reloads.end(); ++itr){
			DBUG_LO("		loading " << itr->second->fig->name());
			itr->second->fig->loadEat( &itr->second->data, &reloads );
		}

		mRoot.mD = reloads[rootSig]->fig;

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

cAnchor::cAnchor():
	mRoot(gWorld.get()->getEmptyFig())
{}

cAnchor::~cAnchor() {
}

void
cAnchor::run(cContext* pCon) {
	start(pCon);
	mRoot.mD->run(pCon);
	stop(pCon);
}

void
cAnchor::patSetRoot(ptrLead aLead){
	mRoot = aLead->getPlug(cAnchor::xPT_root);
}

void
cAnchor::patGetRoot(ptrLead aLead){
	aLead->addPlug(&mRoot, cAnchor::xPT_root);
}


////////////////////////////////////////////////////////////

#ifdef GTUT

class cSaveTester: public cFigment, private tOutline<cSaveTester>{
public:
	static const cPlugTag *xPT_str, *xPT_num;
	static const cCommand::dUID	xGetData;

	cSaveTester(){}
	cSaveTester(const dTextChar* inStr) : myStr(inStr), myNum(42) {}
	virtual ~cSaveTester(){}

	static const dNatChar* identify(){ return "save tester"; }
	virtual const dNatChar* name() const{ return cSaveTester::identify(); }
	virtual dNameHash hash() const{ return tOutline<cSaveTester>::hash(); }

	virtual void save(cByteBuffer* pAddHere) {
		myStr.save(pAddHere); myNum.save(pAddHere);
	}

	virtual void loadEat(cByteBuffer* pBuff, dReloadMap* pReloads = NULL) {
		myStr.loadEat(pBuff, pReloads); myNum.loadEat(pBuff, pReloads);
	}
private:
	tPlug<dText> myStr;
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

cByteBuffer buff;
const dNatChar *testStr = "proper job";

GTUT_START(testAnchor, basicSave){
	tOutline<cSaveTester>::draft();
	tOutline<cAnchor>::draft();
	cContext fakeCon;
	ptrFig ank = gWorld.get()->makeFig(getHash<cAnchor>());
	tPlug<ptrFig> tester(
		ptrFig(new cSaveTester( NCStrToText(testStr).data() ))
	);
	ptrLead add = gWorld.get()->makeLead(cAnchor::xSetRoot, fakeCon.getSig());

	add->addPlug(&tester, cAnchor::xPT_root);
	ank->jack(add, &fakeCon);

	buff.clear();
	ank->save(&buff);
}GTUT_END;

GTUT_START(testAnchor, basicLoad){
	ptrFig ank = gWorld.get()->makeFig(getHash<cAnchor>());
	cContext fake;

	ptrLead load = gWorld.get()->makeLead(cAnchor::xLoad, fake.getSig());
	dReloadMap dontcare;
	ank->loadEat(&buff, &dontcare);

	ptrLead root = gWorld.get()->makeLead(cAnchor::xGetRoot, fake.getSig());
	ank->jack(root, &fake);
	tPlug<ptrFig> reload;
	reload = root->getPlug(cAnchor::xPT_root);

	ptrLead checkData = gWorld.get()->makeLead(cSaveTester::xGetData, fake.getSig());
	reload.mD->jack(checkData, &fake);

	tPlug<dText> myStr = checkData->getPlug(cSaveTester::xPT_str);
	tPlug<int> myNum = checkData->getPlug(cSaveTester::xPT_num);

	GTUT_ASRT(true, "saved strings are not the same");
	GTUT_ASRT(myNum.mD==42, "saved numbers are not the same");
}GTUT_END;


#endif
