#include "ptrTools.hpp"

////////////////////////////////////////////////////////////
// Tests
using namespace gt;

#ifdef GTUT

	GTUT_START(lemmingSpitter, inWild){
		tSpitLemming<int> spitTest;
		{
			tLemming<int> lemA(spitTest.get());
			GTUT_ASRT(spitTest.inWild()==1, "incorrect number of lemmings reported to be in the wild.");
		}
		GTUT_ASRT(spitTest.inWild()==0, "incorrect number of lemmings reported to be in the wild.");
	}GTUT_END;

#endif
