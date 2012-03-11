#include "threadTools.hpp"

using namespace gt;

#ifdef GT_THREADS
	bool isMultithreading::xThreading = false;
#endif

////////////////////////////////////////////////////////////
// Tests
using namespace gt;


#if defined(GTUT) && defined(GT_THREADS)

	GTUT_START(testMrSafety, dataHandling){
		tMrSafety<int> safetyTest;
		const int magicNum = 3;

		safetyTest.set(magicNum);
		GTUT_ASRT(*safetyTest.get().get()==magicNum , "did not get the right number back from mr safety");
	}GTUT_END;



	class testFooData{
	public:
		static const std::string quote;
		static const size_t strLen = 128;
		char testStr[strLen];

		testFooData(){ ::memset(testStr, 0, strLen); }
		~testFooData() { std::cout << "cleaned up data" << std::endl; }

		static void threadFoo(tMrSafety<testFooData> *refData){
			for(size_t idx=0; idx < quote.size(); ++idx){
				refData->get()->testStr[idx] = quote.c_str()[idx];
				#ifndef GT_SPEEDTEST
					boost::this_thread::sleep(boost::posix_time::milliseconds(10));
				#endif
			}
		}
	};
	const std::string testFooData::quote = "remember when I took that wine making course and forgot how to drive?";


	GTUT_START(testMrSafety, onewayData){
		for(int testNum=0; testNum < 3; ++testNum){
			tMrSafety<testFooData> safetyTest;
			std::string whatIGot;
			char o = 0;
			size_t idx=0;

			if(testNum == 2)
				safetyTest.take(new testFooData());
			else
				safetyTest.take(new testFooData);

			boost::thread myTestTread(testFooData::threadFoo, &safetyTest);
			while(idx < testFooData::quote.size() && o != '?'){
				do{
					o = safetyTest.get()->testStr[idx];
				}while(o==0);
				whatIGot.append(1, o);
				++idx;
			}
			myTestTread.join();

			GTUT_ASRT(whatIGot.compare(testFooData::quote)==0, "didn't get back the expected phrase");
		}
	}GTUT_END;

#endif


