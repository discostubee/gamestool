
#include "unitTestFigments.hpp"

#if defined(DEBUG) && defined(GTUT)
	using namespace gt;

	////////////////////////////////////////////////////////////
	const cPlugTag *cTestNum::xPT_num = tOutline<cTestNum>::makePlugTag("number");

	const cCommand::dUID	cTestNum::xGetData = tOutline<cTestNum>::makeCommand(
		"getData", &cTestNum::patGetData, cTestNum::xPT_num, NULL
	);

	const cCommand::dUID	cTestNum::xSetData = tOutline<cTestNum>::makeCommand(
		"setData", &cTestNum::patSetData, cTestNum::xPT_num, NULL
	);

	cTestNum::cTestNum() : myNum(42) {
	}

	cTestNum::~cTestNum(){
	}

	void cTestNum::save(cByteBuffer* pAddHere) {
		myNum.save(pAddHere);
	}

	void cTestNum::loadEat(cByteBuffer* pBuff, dReloadMap* pReloads) {
		myNum.loadEat(pBuff, pReloads);
	}

	void cTestNum::patGetData(ptrLead aLead){
		aLead->addPlug(&myNum, xPT_num);
	}

	void cTestNum::patSetData(ptrLead aLead){
		aLead->getPlug(&myNum, xPT_num);
	}

	////////////////////////////////////////////////////////////
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

#endif

