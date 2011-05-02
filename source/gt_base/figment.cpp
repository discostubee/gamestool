#include "figment.hpp"


////////////////////////////////////////////////////////////
using namespace gt;

const cPlugTag* cFigment::xPT_buffer = tOutline<cFigment>::makePlugTag("buffer");

const cCommand* cFigment::xSave = tOutline<cFigment>::makeCommand(
	"save",
	cFigment::eSave,
	cFigment::xPT_buffer,
	NULL
);

const cCommand* cFigment::xLoad = tOutline<cFigment>::makeCommand(
	"load",
	cFigment::eLoad,
	cFigment::xPT_buffer,
	NULL
);

cFigment::cFigment(){
	//DBUG_LO("...making figment " << reinterpret_cast<unsigned int>(this))
	DBUG_TRACK_START("figment");
}

cFigment::~cFigment(){
	//DBUG_LO("...destroying figment " << reinterpret_cast<unsigned int>(this))
	DBUG_TRACK_END("figment");
}

void
cFigment::jack(ptrLead pLead){
	PROFILE;

	try{
		switch( pLead->mCom->getSwitch<cFigment>() ){
			case eSave:{
				cPlug<cByteBuffer>* saveBuff = new cPlug<cByteBuffer>;
				saveBuff->mD = save();
				pLead->take(saveBuff, cFigment::xPT_buffer );
			}break;

			case eLoad:{
				loadEat( pLead->getD(cFigment::xPT_buffer)->getMDPtr<cByteBuffer>() );
			}break;

			default:
			case eNotMyBag:{
				DBUG_LO("not my bag");
			}break;
		}
	}catch(excep::base_error &e){
		WARN(e);
	}catch(...){
		UNKNOWN_ERROR;
	}
}

void
cFigment::run(cContext* pCon){
	//DBUG_LO( "running " << name() );
}

cByteBuffer&
cFigment::save(){
	cByteBuffer* empty = new cByteBuffer();
	return *empty;
}

void 
cFigment::requirements(){
}

////////////////////////////////////////////////////////////
using namespace gt;

cEmptyFig::cEmptyFig(){
	//DBUG_LO("*** empty fig made " << reinterpret_cast<unsigned int>(this));
}

cEmptyFig::~cEmptyFig(){
	//DBUG_LO("*** empty fig destroyed! " << reinterpret_cast<unsigned int>(this));
}

////////////////////////////////////////////////////////////
using namespace gt;

cWorldShutoff::cWorldShutoff(){
}

cWorldShutoff::~cWorldShutoff(){
}

void
cWorldShutoff::run(cContext* pCon){
	gWorld->mKeepLooping = false;
}
