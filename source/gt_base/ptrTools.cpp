#include "ptrTools.hpp"

////////////////////////////////////////////////////////////
// Tests
using namespace gt;

#ifdef GTUT
	class spitInts: public tSpitLemming<int>{
	public:
		spitInts(){}
		virtual ~spitInts(){}

		void setData(int in) { myData=in; }

		virtual int* getData(const tLemming<int>* requester){
			return &myData;
		}

	protected:
		virtual int* getData() { return &myData; }

	private:
		int myData;

	};

	GTUT_START(lemmingSpitter, dataHandling){
		spitInts testSpit;
		const int magicNum = 3;

		testSpit.setData(magicNum);
		GTUT_ASRT(*(testSpit.get().get())==magicNum , "did not get the right number back from spitter");
	}GTUT_END;

	GTUT_START(lemmingSpitter, inWild){
		tSpitLemming<int> spitTest;
		{
			tLemming<int> lemA = spitTest.get();
			GTUT_ASRT(spitTest.inWild()==1, "incorrect number of lemmings reported to be in the wild.");
		}
		GTUT_ASRT(spitTest.inWild()==0, "incorrect number of lemmings reported to be in the wild.");

		{
			tLemming<int> lemA = spitTest.get();
			tLemming<int> lemB = lemA;
			GTUT_ASRT(spitTest.inWild()==1, "incorrect number of lemmings reported to be in the wild.");
		}
		GTUT_ASRT(spitTest.inWild()==0, "incorrect number of lemmings reported to be in the wild.");
	}GTUT_END;

	struct pmorphParent {
		int num;
		virtual int magic(){ return 3; }
	};

	struct pmorphChild : public pmorphParent {
		virtual int magic(){ return 42; }
	};

	GTUT_START(test_pmorph, something){
		tPMorphJar<pmorphParent> jarA;

		{
			pmorphParent parent;
			parent.num = 2;
			jarA = parent;
		}
		GTUT_ASRT(jarA.get().num == 2, "jar didn't get int right");

		{
			pmorphChild child;
			child.num = 5;
			jarA = child;
		}
		GTUT_ASRT(jarA.get().num == 5, "jar didn't get int right");
		GTUT_ASRT(jarA.get().magic() == 42, "jar didn't get polymorph right");

	}GTUT_END;

#endif
