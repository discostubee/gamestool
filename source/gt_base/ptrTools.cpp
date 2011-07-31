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

#endif
