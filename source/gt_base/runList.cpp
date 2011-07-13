#include "runList.hpp"

////////////////////////////////////////////////////////////
using namespace gt;

const cCommand* cRunList::xAdd = tOutline<cRunList>::makeCommand(
	"add",
	cRunList::eAdd,
	NULL
);

cRunList::cRunList(){
}

cRunList::~cRunList(){
}

void
cRunList::run(cContext* pCon){
	PROFILE;

	ASRT_NOTNULL(pCon);

	if(pCon->isStacked(this))
		return;	

	pCon->add(this);

	for(
		std::vector< cPlug<ptrFig> >::iterator i = mList.begin();
		i != mList.end();
		++i
	){
		(*i).mD->run(pCon);
	}
	pCon->finished(this);
}

void
cRunList::jack(ptrLead pLead){
	PROFILE;

	try{
		switch( pLead->mCom->getSwitch<cRunList>() ){
			case eAdd: {
				cPlug<ptrFig> temp;

				for(cLead::cPileItr itr = pLead->getPiledDItr(); !itr.atEnd(); ++itr){
					temp = *itr.getPlug();
					mList.push_back(temp);
					DBUG_LO("	RunList added a " << temp.mD->name() );
				};

			}break;

			default:{
				cFigment::jack(pLead);
			}break;
		}
	}catch(excep::base_error &e){
		WARN(e);
	}
}

void
cRunList::save(cByteBuffer* pAddHere){
	PROFILE;

	size_t listSize = mList.size();

	pAddHere->add( &listSize );
	for( std::vector< cPlug<ptrFig> >::iterator i = mList.begin(); i != mList.end(); ++i){
		i->save(pAddHere);
	}
}

void
cRunList::loadEat(cByteBuffer* pBuff, dReloadMap* pReloads){
	PROFILE;

	ASRT_NOTNULL(pBuff);
	ASRT_NOTNULL(pReloads);

	size_t listSize;
	pBuff->fill(&listSize);
	pBuff->trimHead(sizeof listSize);
	for(size_t i = 0; i < listSize; ++i){
		cPlug<ptrFig> tempFig;
		tempFig.loadEat(pBuff, pReloads); // there should be enough in this buffer for all figs to load.
		mList.push_back(tempFig);
	}
}

void
cRunList::getLinks(std::list<ptrFig>* pOutLinks){
	ASRT_NOTNULL(pOutLinks);

	for(
		std::vector< cPlug<ptrFig> >::iterator i = mList.begin();
		i != mList.end();
		++i
	){
		pOutLinks->push_back( i->mD );
	}
}

////////////////////////////////////////////////////////////
using namespace gt;

const cPlugTag* cValves::xPT_state = tOutline<cValves>::makePlugTag("valve state");
const cPlugTag* cValves::xPT_valveIdx = tOutline<cValves>::makePlugTag("Valve index");

const cCommand* cValves::xSetState = tOutline<cValves>::makeCommand(
	"add",
	cValves::eSetState,
	NULL
);

cValves::cValves(){
}

cValves::~cValves(){
}

void
cValves::run(cContext* pCon){
	PROFILE;

	ASRT_NOTNULL(pCon);

	if(pCon->isStacked(this))
		return;

	pCon->add(this);

	for(
		std::vector< cPlug<ptrFig> >::iterator itr = mList.begin();
		itr != mList.end();
		++itr
	){
		if( mStates[itr].mD )
			(*itr).mD->run(pCon);
	}
	pCon->finished(this);
}

void
cValves::jack(ptrLead pLead){
	PROFILE;

	try{
		switch( pLead->mCom->getSwitch<cValves>() ){
			case eAdd:{
				cPlug<ptrFig> temp;

				for(cLead::cPileItr itr = pLead->getPiledDItr(); !itr.atEnd(); ++itr ){
					temp = *itr.getPlug();
					mList.push_back(temp);
					mStates[mList.end() -1] = false;
					DBUG_LO("	Valves added a " << temp.mD->name() );
				};
			}break;

			case eSetState:{
				dListItr tempItr = mList.begin() + pLead->getD(xPT_valveIdx)->getMDCopy<unsigned int>();
				mStates[ tempItr ] = pLead->getD(xPT_state)->getMDCopy<bool>();
			}break;

			default:{
				cFigment::jack(pLead);
			}break;
		}
	}catch(excep::base_error &e){
		WARN(e);
	}
}

void
cValves::save(cByteBuffer* pAddHere){
	//!\todo add saves for each valve
	cRunList::save(pAddHere);
}

void
cValves::loadEat(cByteBuffer* pBuff, dReloadMap* pReloads){

	//!\todo, add in your stuff here.

	cRunList::loadEat(pBuff, pReloads);
}

