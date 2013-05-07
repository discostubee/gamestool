//
//#include "glassHouse.hpp"
//
//using namespace gt;
//
//////////////////////////////////////////////////////////////
//
//#ifdef GTUT
//
//struct testHouse{
//	STAMP_POT(testHouse, int, A);
//	STAMP_POT(testHouse, unsigned short, B);
//	STAMP_POT(testHouse, bool, C);
//
//	testHouse& operator += (const testHouse &copy){ return *this; }
//};
//
//typedef tGlassHouse<testHouse> ghTest;
//
//template<> ghTest::dName2Pot
//ghTest::xPots = ghTest::eatPotsToMake(
//	new ghTest::cPot("pot A", &testHouse::setPotA, &testHouse::getPotA),
//	new ghTest::cPot("pot B", &testHouse::setPotB, &testHouse::getPotB),
//	new ghTest::cPot("pot C", &testHouse::setPotC, &testHouse::getPotC),
//	NULL
//);
//
//
//GTUT_START(testGlassHouse, setMember){
//	ghTest testMe;
//	tPlug<int> plugA;
//	tPlug<unsigned short> plugB;
//	tPlug<bool> plugC;
//
//	testMe.A = 3;
//	testMe.B = 42;
//	testMe.C = true;
//
//	testMe.get(&plugA, makeHash("pot A"));
//	testMe.get(&plugB, makeHash("pot B"));
//	testMe.get(&plugC, makeHash("pot C"));
//
//	GTUT_ASRT(plugA.get()==testMe.A, "A not right");
//	GTUT_ASRT(plugB.get()==testMe.B, "B not right");
//	GTUT_ASRT(plugC.get()==testMe.C, "C not right");
//}GTUT_END;
//
//GTUT_START(testGlassHouse, getMember){
//	ghTest testMe;
//	tPlug<int> plugA;
//	tPlug<unsigned short> plugB;
//	tPlug<bool> plugC;
//
//	plugA = 3;
//	plugB = 42;
//	plugC = true;
//
//	testMe.set(&plugA, makeHash("pot A"));
//	testMe.set(&plugB, makeHash("pot B"));
//	testMe.set(&plugC, makeHash("pot C"));
//
//	GTUT_ASRT(testMe.A==plugA.get(), "A not right");
//	GTUT_ASRT(testMe.B==plugB.get(), "B not right");
//	GTUT_ASRT(testMe.C==plugC.get(), "C not right");
//}GTUT_END;
//
//GTUT_START(testGlassHouse, failures){
//	bool failed=false;
//
//	try{
//
//	}catch(...){
//		failed = true;
//	}
//}GTUT_END;
//
//#endif
