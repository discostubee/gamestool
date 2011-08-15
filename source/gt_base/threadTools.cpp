#include "threadTools.hpp"

using namespace gt;

#ifdef GT_THREADS
	bool isMultithreading::xThreading = false;
#endif

////////////////////////////////////////////////////////////
// Tests
using namespace gt;

#ifdef OFF
namespace demo{
	typedef boost::unique_lock<boost::mutex> dLock;
	boost::mutex myMutex;
	dLock waitLock;
	boost::condition_variable syncIt;

	void testThread(){
		dLock myLock;
		dLock unused;
		myLock = dLock(myMutex);
		syncIt.notify_all();
		std::cout << "thread sleeping" << std::endl;
		sleep(2);
		std::cout << "thread finished" << std::endl;
		return;
	}

	void testSwapMutex(){
		boost::mutex A, B;
		bool AbeforeB, AafterB;
		{
			dLock myLock(A);
			AbeforeB = A.try_lock();
			myLock = dLock(B);
			AafterB = A.try_lock();
			A.unlock();
		}
		return;
	}

	void threadTestFoo(){
		testSwapMutex();

		waitLock = dLock(myMutex);
		boost::thread myThread(testThread);
		syncIt.wait(waitLock);
		std::cout << "main finished waiting" << std::endl;
		myThread.join();
		return;
	}
}
#endif

#if defined(GTUT) && defined(GT_THREADS)

	GTUT_START(testMrSafety, dataHandling){
		tMrSafety<int> safetyTest;
		const int magicNum = 3;

		safetyTest.set(magicNum);
		GTUT_ASRT(*safetyTest.get().get()==magicNum , "did not get the right number back from mr safety");
	}GTUT_END;

	class wildCounter : public tMrSafety<int> {
	public:
		wildCounter() {}
		~wildCounter() {}

		int count() { return inTheWild; }
	};

	void threadFooWild(wildCounter *counter){

	}

	GTUT_START(testMrSafety, inTheWild){
		wildCounter testC;
		int *data = new int;
		*data = 1;

		testC.take(data);

		for(int i=0; i < 32; ++i)
			*testC.get().get() += 1;

		GTUT_ASRT(testC.count()==0, "wild count incorrect.");
	}GTUT_END;

	class testFooData{
	public:
		static const std::string quote;
		static const size_t strLen = 128;
		char testStr[strLen];

		testFooData() { ::memset(testStr, 0, strLen); }
		~testFooData() { std::cout << "cleaned up data" << std::endl; }

		static void threadFoo(tMrSafety<testFooData> *refData){
			for(size_t idx=0; idx < quote.size(); ++idx){
				refData->get()->testStr[idx] = quote.c_str()[idx];
				std::cout << ".";
			}
		}
	};
	const std::string testFooData::quote = "remember when I took that wine making course and forgot how to drive?";


	GTUT_START(testMrSafety, onewayData){
		tMrSafety<testFooData> safetyTest;
		std::string whatIGot;
		char o = 0;
		size_t idx=0;

		safetyTest.take(new testFooData);
		boost::thread myTestTread(testFooData::threadFoo, &safetyTest);
		while(idx < testFooData::quote.size() && o != '?'){
			do{
				o = safetyTest.get()->testStr[idx];
			}while(o==0);
			std::cout << o;
			whatIGot.append(1, o);
			++idx;
		}
		myTestTread.join();

		GTUT_ASRT(whatIGot.compare(testFooData::quote)==0, "");
	}GTUT_END;

#endif


