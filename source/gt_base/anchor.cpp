#include "anchor.hpp"

////////////////////////////////////////////////////////////
using namespace gt;

const cPlugTag* cAnchor::xPT_root = tOutline<cAnchor>::makePlugTag("root");

const cCommand* cAnchor::xSetRoot = tOutline<cAnchor>::makeCommand(
	"set root",
	cAnchor::eSetRoot,
	cAnchor::xPT_root,
	NULL
);

const cCommand* cAnchor::xGetRoot = tOutline<cAnchor>::makeCommand(
	"get root",
	cAnchor::eGetRoot,
	cAnchor::xPT_root,
	NULL
);

cByteBuffer&
cAnchor::save(){
	PROFILE;

	cByteBuffer*		saveBuff = new cByteBuffer();
	ptrLead				append(new cLead(cBase_fileIO::xInsert));
	std::list<ptrFig>*	branches = new std::list<ptrFig>();
	std::list<ptrFig>*	prev = new std::list<ptrFig>();
	std::set<iFigment*> figs;

	size_t				chunkSize = 0;
	dNameHash			chunkHash = 0;
	cPlug<size_t>		currentSpot = 0;
	dFigSaveSig			chunkSig = 0;

	try{
		mRoot.mD->getLinks(branches);
		figs.insert( mRoot.mD.get() );

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
					figs.insert( i->get() );
					(*i)->getLinks( branches );	//add to the next series of branches.
				}
			}
		}while(branches->size() > 0);

		DBUG_LO(":) anchor saving " << figs.size() << " figments" );

		chunkSig = mRoot.mD.get();
		saveBuff->add( &chunkSig ); // Save reference to the root.

		for( std::set<iFigment*>::iterator i = figs.begin(); i != figs.end(); ++i ){
			cByteBuffer	chunkSave;

			DBUG_LO("	saving a " << (*i)->name());

			//- Get additional save data.
			try{
				chunkSave = (*i)->save();
			}catch(excep::dontUseThis){
			}

			//- Get the hash.
			chunkHash = (*i)->getReplacement();

			if( chunkHash == uDoesntReplace ){
				chunkHash = (*i)->hash();
			}

			//-
			chunkSize = sizeof(dNameHash) + sizeof(dFigSaveSig) + chunkSave.size();

			chunkSig = *i;

			//- Add it to the buffer.
			saveBuff->add( &chunkSize );
			saveBuff->add( &chunkHash );
			saveBuff->add( &chunkSig );			//- use the pointer as a way to identify it when referencing figment pointers want to reload.
			if(chunkSave.size() > 0)
				saveBuff->add(chunkSave);
		}
		
	}catch(...){
		delete branches;
		delete prev;
		delete saveBuff;

		throw;
	}

	delete branches;
	delete prev;

	return *saveBuff;
}

void
cAnchor::loadEat(cByteBuffer* pBuff, dReloadMap* pReloads){
	PROFILE;

	static const size_t BLOCK_SIZE = sizeof(size_t) + sizeof(dNameHash) + sizeof(dFigSaveSig);

	dReloadMap		reloads;
	dFigSaveSig		rootSig;
	size_t			chunkSize;
	dNameHash		tempHash;
	dFigSaveSig		reloadSig;

	ASRT_NOTNULL(pBuff);
	ASRT_NOTNULL(pReloads);

	try{
		DBUG_LO(":) anchor loading");

		pBuff->fill(&rootSig); //- the first entry in the file is the root signature.
		pBuff->trim(sizeof rootSig);

		while(pBuff->size() >= BLOCK_SIZE){
			pBuff->fill(&chunkSize); //- first, let's get the size of this entire chunk.
			pBuff->trim(sizeof chunkSize);

			pBuff->fill(&tempHash);	//- now, let's get the name hash and spawn the figment.
			pBuff->trim(sizeof tempHash);

			pBuff->fill(&reloadSig); //- next up we take the signature of this figment which will be used when figments reference each other.
			pBuff->trim(sizeof reloadSig);

			chunkSize -= (sizeof(chunkSize) + sizeof(tempHash));

			reloads[reloadSig] = new cReload(	//- lastly, lets get data for re-loading the figment.
				ptrFig(gWorld->makeFig(tempHash)),
				pBuff->get(),
				chunkSize
			);

			pBuff->trim(chunkSize);
		}

		//- Now re-load all the figs we've made. It's done on a separate loop to the on above so that figment references can be re-created.
		for(dReloadMap::iterator itr = reloads.begin(); itr != reloads.end(); ++itr){
			DBUG_LO("		loading " << itr->second->fig->name());
			itr->second->fig->loadEat( &itr->second->data, &reloads );
		}

		mRoot.mD = reloads[rootSig]->fig;

		for(dReloadMap::iterator itr = reloads.begin(); itr != reloads.end(); ++itr){	//- Cleanup
			SAFEDEL(itr->second);
		}

	}catch(...){
		for(dReloadMap::iterator itr = reloads.begin(); itr != reloads.end(); ++itr){	//- Cleanup
			SAFEDEL(itr->second);
		}
		throw;
	}


	//- Old barfing buffer version.
	//	dReloadMap				reloadBunch;
	//	dFigSaveSig*			rootSig = NULL;
	//	size_t*					chunkSize = NULL;
	//	dNameHash*				tempHash = NULL;
	//	dFigSaveSig*			reloadSig = NULL;
	//
	//	ASRT_NOTNULL(pBuff);
	//	ASRT_NOTNULL(pReloads);
	//
	//	try{
	//
	//		DBUG_LO(":) anchor loading");
	//
	//		rootSig = (dFigSaveSig*)pBuff->barf(sizeof(dFigSaveSig));
	//
	//		while(pBuff->size() >= sizeof(size_t) + sizeof(dNameHash) + sizeof(dFigSaveSig) ){
	//			DBUG_VERBOSE_LO(" 	buff size " << pBuff->size());
	//
	//			cReload* newReload = new cReload;	// cleaned up by map.
	//
	//			//- first, let's get the size of this entire chunk.
	//			chunkSize = (size_t*)pBuff->barf(sizeof(size_t));
	//			DBUG_VERBOSE_LO("	chunk size " << *chunkSize);
	//
	//			tempHash = (dNameHash*)pBuff->barf(sizeof(dNameHash));	//- now, let's get the name hash and spawn the figment.
	//			reloadSig = (dFigSaveSig*)pBuff->barf(sizeof(dFigSaveSig));	//- next up we take the signature of this figment which will be used when figments reference each other.
	//			*chunkSize -= (sizeof(dNameHash) + sizeof(dFigSaveSig));	//- lastly, lets get data for re-loading the figment.
	//
	//			DBUG_VERBOSE_LO("	save data size " << *chunkSize);
	//
	//			if( *chunkSize > 0 ){
	//				newReload->data.take(
	//					static_cast<dByte*>(pBuff->barf(*chunkSize)),
	//					*chunkSize
	//				);
	//			}
	//
	//			try{
	//				newReload->fig = gWorld->makeFig(*tempHash);
	//				reloadBunch[*reloadSig] = newReload;
	//			}catch(...){
	//				//do something.
	//			}
	//
	//			delete chunkSize;	chunkSize = NULL;
	//			delete tempHash;	tempHash = NULL;
	//			delete reloadSig;	reloadSig = NULL;
	//
	//			DBUG_VERBOSE_LO("	chunk done");
	//		}
	//
	//		//- Now re-load all the figs we've made. It's done on a separate loop to the on above so that figment references can be re-created.
	//		for(dReloadMap::iterator itr = reloadBunch.begin(); itr != reloadBunch.end(); ++itr){
	//			DBUG_LO("		loading " << itr->second->fig->name());
	//			itr->second->fig->loadEat( &itr->second->data, &reloadBunch );
	//		}
	//
	//		mRoot.mD = reloadBunch[*rootSig]->fig;
	//
	//		// cleanup the reload data.
	//		for(dReloadMap::iterator itr = reloadBunch.begin(); itr != reloadBunch.end(); ++itr){
	//			delete itr->second;
	//			itr->second = NULL;
	//		}
	//
	//		delete rootSig;
	//
	//	}catch(...){
	//		for(dReloadMap::iterator itr = reloadBunch.begin(); itr != reloadBunch.end(); ++itr){
	//			SAFEDEL(itr->second);
	//		}
	//
	//		SAFEDEL(chunkSize);
	//		SAFEDEL(tempHash);
	//		SAFEDEL(reloadSig);
	//		SAFEDEL(rootSig);
	//		throw;
	//	}
}

cAnchor::cAnchor():
	mRoot(gWorld->getEmptyFig())
{
	
}

cAnchor::~cAnchor() {
}

void
cAnchor::requirements(){
	//tOutline<cFigment>::draft();
}

void
cAnchor::run(cContext* pCon) {
	ASRT_NOTNULL(pCon);

	mRoot.mD->run(pCon);
}

void
cAnchor::jack(ptrLead pLead) {
	try{
		switch( pLead->mCom->getSwitch<cAnchor>() ){
			case eSetRoot:
				mRoot = *pLead->getD(cAnchor::xPT_root);
				break;

			case eGetRoot:
				pLead->add( &mRoot, cAnchor::xPT_root );
				break;

			default:
				cFigment::jack(pLead);
				break;
		}
	}catch(excep::base_error &e){
		WARN(e);
	}
}
