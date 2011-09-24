//!\file	main.cpp
//!\brief	


// Include the stuff we want to test.
#include <gt_base/figment.hpp>


//-----------------------------------------------
// Testing out ideas.
/*
namespace testZone{

	typedef int dData;
	typedef dNameHash funkHand;
	class iMan;
	template<typename T> class tMan;

	class lead;

	class iFunk{	// will be command
	public:

		virtual ~iFunk() {}

		virtual const std::string& getName() const =0;
		virtual void call(iMan *funkWith, lead *aLead) =0;
	};

	class iMan{	// will be outline
	public:
		typedef std::map<dNameHash, iFunk* > funkmap;

		virtual ~iMan() {}
		virtual std::vector<iFunk*> getAllMyFoo() = 0;
	};

	class lead{
	public:
		const funkHand hand;

		lead(funkHand aHand) : hand(aHand) {}
		~lead() {}

		dData getD(int ID) {return 0;}
		void addD(dData) {}

	private:

	};

	template <class T>
	class funk: public iFunk{	// will be command
	public:
		typedef void (T::*fooPtr)(lead *aLead);

		funk();
		funk(std::string aName, fooPtr aFoo);
		virtual ~funk();

		virtual const std::string& getName() const;
		virtual void call(iMan *funkWith, lead *aLead);

		std::string name;
		fooPtr myFoo;
	};

	template<typename T>
	class tMan: public iMan{	// will be outline
	public:

		static funkHand makeFunk(const char *name, typename funk<T>::fooPtr foo, bool unmake=false);
		template<typename PARENT> static void extend();
		static void cleanup();

		tMan();
		virtual ~tMan();
		virtual std::vector<iFunk*> getAllMyFoo();

		static funkmap *allFunk;

	private:
		typename funkmap::iterator itrTmp;
	};

	////////////////////////////////////////////////////////////

	template<typename T>
	funk<T>::funk(): name("bad"), myFoo(NULL) {
	}

	template<typename T>
	funk<T>::funk(std::string aName, fooPtr aFoo):
		name(aName), myFoo(aFoo)
	{}

	template<typename T>
	funk<T>::~funk() {

	}

	template<typename T>
	const std::string&
	funk<T>::getName() const {
		return name;
	}

	template<typename T>
	void
	funk<T>::call(iMan *funkWith, lead *aLead){
		T *tmp = reinterpret_cast< T* >(funkWith);
		( tmp->*myFoo )(aLead);
	}

	////////////////////////////////////////////////////////////

	template<typename T>
	typename tMan<T>::funkmap *tMan<T>::allFunk;	// DONT ALLOCATE ANYTHING

	template<typename T>
	tMan<T>::tMan(){
	}

	template<typename T>
	tMan<T>::~tMan(){
	}

	template<typename T>
	funkHand
	tMan<T>::makeFunk(const char *name, typename funk<T>::fooPtr foo, bool unmake){
		static bool madeAllfunk = false;

		if(!unmake){
			if(!madeAllfunk){
				allFunk = new funkmap();
				madeAllfunk = true;
			}
			dNameHash tmpHash = makeHash(name);
			allFunk->insert( std::pair<dNameHash, iFunk*>(tmpHash, new funk<T>(name, foo)) );
			return tmpHash;

		}else if(madeAllfunk){
			delete allFunk;
		}

		return 0;
	}

	template<typename T>
	template<typename PARENT>
	void
	tMan<T>::extend(){

		for(
			typename tMan<PARENT>::funkmap::iterator itr = tMan<PARENT>::allFunk->begin();
			itr != tMan<PARENT>::allFunk->end();
			++itr
		){
			funk<PARENT> *tmp = reinterpret_cast< funk<PARENT>* >(itr->second);
			tMan<T>::allFunk->insert(
				std::pair<dNameHash, iFunk*>(
					itr->first, new funk<T>(tmp->name, tmp->myFoo)
				)
			);
		}

	}

	template<typename T>
	void
	tMan<T>::cleanup(){
		for(typename funkmap::iterator itr = allFunk->begin(); itr != allFunk->end(); ++itr)
			delete itr->second;

		allFunk->clear();
		makeFunk(NULL, NULL, true);
		allFunk = NULL;
	}

	template<typename T>
	std::vector<iFunk*>
	tMan<T>::getAllMyFoo(){
		std::vector<iFunk*> allFunk;
		return allFunk;
	}

	////////////////////////////////////////////////////////////

	class testA: private tMan<testA>{
	public:
		static const funkHand fooA;
		static const funkHand fooB;

		funkmap *myFunk;
		std::string me;

		testA(){

		}
		virtual ~testA(){}

		virtual funkmap * getMyFunk(){
			return tMan<testA>::allFunk;
		}

		void jack(lead *aLead){
			itrTmp = myFunk->find(aLead->hand);
			if(itrTmp == myFunk->end())
				throw std::exception();

			itrTmp->second->call(this, aLead);
		}

		void doFooA(lead *aLead){ std::cout << me << " doing A" << std::endl; }
		void doFooB(lead *aLead){ std::cout << me << " doing B" << std::endl; }
	};

	const funkHand testA::fooA = tMan<testA>::makeFunk("do foo A", &testA::doFooA);
	const funkHand testA::fooB = tMan<testA>::makeFunk("do foo B", &testA::doFooB);

	class testAChild: public testA, private tMan<testAChild>{
	public:
		static const funkHand fooC;

		testAChild() {}
		virtual ~testAChild(){}

		virtual funkmap * getMyFunk(){
			return tMan<testA>::allFunk;
		}

		void doFooC(lead *aLead){ std::cout << me << " doing C" << std::endl; }
	};

	const funkHand testAChild::fooC = tMan<testAChild>::makeFunk("do foo C", &testAChild::doFooC);

	void
	runTest(){
		testA A;
		testAChild B;
		lead leadFooA(testA::fooA), leadFooB(testA::fooB), leadFooC(testAChild::fooC);

		tMan<testAChild>::extend<testA>(); // could be done as part of a draft.

		A.me = "parent";
		B.me = "child";

		A.jack(&leadFooA);
		B.jack(&leadFooA);
		A.jack(&leadFooB);
		B.jack(&leadFooC);
		try{ A.jack(&leadFooC); }catch(std::exception){ std::cout << "caught like a boss" << std::endl; }
	}

}
*/



int
main(int argc, char **argv){
	int result = EXIT_FAILURE;

	std::cout << "Running gamestool tests. Version 0.1" << std::endl;

	//testZone::runTest();

	gt::gWorld.take( new gt::cWorld() );

#ifdef GTUT_GOOGLE
	::testing::InitGoogleTest(&argc, argv);
	result = RUN_ALL_TESTS();
#endif

	gt::gWorld.get()->makeProfileReport(std::cout);
	gt::gWorld.cleanup();

	return result;
}

