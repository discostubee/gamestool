#include "runList.hpp"

////////////////////////////////////////////////////////////
using namespace gt;

const cCommand::dUID cRunList::xAdd = tOutline<cRunList>::makeCommand(
	"add", &cRunList::patAdd,
	NULL
);

cRunList::cRunList(){
}

cRunList::~cRunList(){
}

void
cRunList::run(cContext* pCon){
	start(pCon);
	PROFILE;

	for(
		std::vector< tPlug<ptrFig> >::iterator i = mList.begin();
		i != mList.end();
		++i
	){
		(*i).mD->run(pCon);
	}

	stop(pCon);
}

void
cRunList::save(cByteBuffer* pAddHere){
	PROFILE;

	size_t listSize = mList.size();

	pAddHere->add( &listSize );
	for( std::vector< tPlug<ptrFig> >::iterator i = mList.begin(); i != mList.end(); ++i){
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
		tPlug<ptrFig> tempFig;
		tempFig.loadEat(pBuff, pReloads); // there should be enough in this buffer for all figs to load.
		mList.push_back(tempFig);
	}
}

void
cRunList::getLinks(std::list<ptrFig>* pOutLinks){
	ASRT_NOTNULL(pOutLinks);

	for(
		std::vector< tPlug<ptrFig> >::iterator i = mList.begin();
		i != mList.end();
		++i
	){
		pOutLinks->push_back( i->mD );
	}
}

void
cRunList::patAdd(ptrLead aLead){
	PROFILE;

	dList plugs;
	aLead->getPile(&plugs);

	//!\todo use append
	for(dList::iterator itr = plugs.begin(); itr != plugs.end(); ++itr){
		mList.push_back( *itr );
	}
}

////////////////////////////////////////////////////////////
using namespace gt;

const cPlugTag* cValves::xPT_state = tOutline<cValves>::makePlugTag("valve state");
const cPlugTag* cValves::xPT_valveIdx = tOutline<cValves>::makePlugTag("Valve index");

const cCommand::dUID cValves::xSetState = tOutline<cValves>::makeCommand(
	"set valve",
	&cValves::patSetValve,
	NULL
);

cValves::cValves(){
}

cValves::~cValves(){
}

void
cValves::run(cContext* pCon){
	PROFILE;

	start(pCon);

	for(
		std::vector< tPlug<ptrFig> >::iterator itr = mList.begin();
		itr != mList.end();
		++itr
	){
		if( mStates[itr].mD )
			(*itr).mD->run(pCon);
	}

	stop(pCon);
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

void
cValves::patSetValve(ptrLead pLead){
}

