#include "plug.hpp"
#include "figment.hpp"

using namespace gt;


cReload::cReload(){
}

cReload::cReload(ptrFig pFig, const dByte* copyMe, size_t buffSize): fig(pFig){
	if(copyMe != NULL && buffSize > 0)
		data.copy(copyMe, buffSize);
}

cReload::~cReload(){
}

////////////////////////////////////////////////////////////
cBase_plug::cBase_plug(PLUG_TYPE_ID pTI):
	mType(pTI)
{
}

cBase_plug::cBase_plug(const cBase_plug& pCopy):
	mType(pCopy.mType)
{
}

cBase_plug::~cBase_plug(){
	try{
		PROFILE;
		DBUG_VERBOSE_LO("destroying plug");

		while( !mLeadsConnected.empty() ){
			DBUG_VERBOSE_LO( "disconnecting lead " << reinterpret_cast<unsigned int>(*mLeadsConnected.begin()) );
			(*mLeadsConnected.begin())->unplug(this);
			mLeadsConnected.erase(mLeadsConnected.begin());
		}
	}catch(...){
		excep::base_error e("Unknown error when destroying a plug", __FILE__, __LINE__);
		WARN(e);
	}
}

void
cBase_plug::linkLead(cLead* pLead){
	ASRT_NOTNULL(pLead);

	DBUG_VERBOSE_LO("Connecting lead " << reinterpret_cast<unsigned int>(pLead) );
	mLeadsConnected.insert(pLead);
}

void
cBase_plug::unlinkLead(cLead* pLead){
	ASRT_NOTNULL(pLead);

	std::set<cLead*>::iterator tempI = mLeadsConnected.find(pLead);
	if( tempI != mLeadsConnected.end() ){	// there should be no need for this check.
		mLeadsConnected.erase( tempI );

		DBUG_VERBOSE_LO( "disconnecting lead " << reinterpret_cast<unsigned int>(pLead) );
	}
}

cBase_plug&	
cBase_plug::operator= (const cBase_plug &pD){ 
	DUMB_REF_ARG(pD); 
	DONT_USE_THIS; 
	return *this; 
}

cBase_plug&
cBase_plug::operator= (const cBase_plug* pD){
	DUMB_REF_ARG(pD);
	DONT_USE_THIS;
	return *this;
}

////////////////////////////////////////////////////////////
// Tests
#ifdef GTUT

GTUT_START(testPlug, copy)
{
	cPlug<int> A;
	cBase_plug *B = new cPlug<int>();
	int C = 0;
	char D = 'a';
	
	A = 5;
	TRYME( *B = A );

	TRYME( C = B->getMDCopy<int>() );
	GTUT_ASRT(C == 5, "B didn't copy A");

	try{
		D = B->getMDCopy<int>();
	}catch(std::exception){
		GTUT_ASRT(true, "");
	}

	delete(B);
}GTUT_END;

GTUT_START(testPlug, saveLoad){
	cPlug<int> A, B;
	cByteBuffer buff;
	dReloadMap dontCare;

	A.mD = 42;
	A.save(&buff);
	B.loadEat(&buff, &dontCare);
	GTUT_ASRT(B.mD == A.mD, "A didn't save, or B didn't load, correctly.");
}GTUT_END;

#endif