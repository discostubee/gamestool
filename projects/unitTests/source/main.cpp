//!\file	main.cpp
//!\brief	


// Include the stuff we want to test.
#include <gt_base/figment.hpp>

namespace testZone{

typedef int dData;
typedef dNameHash funkHand;
class iMan;
template<typename T> class tMan;

class iFunk{
public:
	virtual ~iFunk() {}

	virtual const std::string& getName() =0;
	virtual void call(iMan *funkWith, std::list<dData> aData) =0;
};

class iMan{
public:
	virtual ~iMan() {}
	virtual std::vector<iFunk*> getAllMyFoo() = 0;
};

template <class T>
class funk: public iFunk{
public:
	typedef void (T::*fooPtr)(std::list<dData>);

	funk();
	funk(std::string aName, fooPtr aFoo);
	virtual ~funk();

	virtual const std::string& getName();
	virtual void call(iMan *funkWith, std::list<dData> aData);

	std::string name;
	fooPtr myFoo;
};

template<typename T>
class tMan: public iMan{
public:
	typedef std::map<dNameHash, iFunk* > funkmap;

	typename funkmap::iterator itrTmp;

	static funkmap *allFunk;

	static funkHand makeFunk(const char *name, typename funk<T>::fooPtr foo, bool unmake=false);
	template<typename PARENT> static void extend();
	static void cleanup();

	tMan();
	virtual ~tMan();
	virtual std::vector<iFunk*> getAllMyFoo();

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
funk<T>::getName(){
	return name;
}

template<typename T>
void
funk<T>::call(iMan *funkWith, std::list<dData> aData){
	T *tmp = reinterpret_cast< T* >(funkWith);
	( tmp->*myFoo )(aData);
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

	testA(): myFunk(tMan<testA>::allFunk) {

	}
	virtual ~testA(){}

	void jack(dNameHash funkID, std::list<dData> aData){
		itrTmp = myFunk->find(funkID);
		if(itrTmp == myFunk->end())
			throw std::exception();

		itrTmp->second->call(this, aData);
	}

	void doFooA(std::list<dData> inData){ std::cout << me << " doing A" << std::endl; }
	void doFooB(std::list<dData> inData){ std::cout << me << " doing B" << std::endl; }
};

const funkHand testA::fooA = tMan<testA>::makeFunk("do foo A", &testA::doFooA);
const funkHand testA::fooB = tMan<testA>::makeFunk("do foo B", &testA::doFooB);

class testAChild: public testA, private tMan<testAChild>{
public:
	static const funkHand fooC;

	testAChild(){
		myFunk = tMan<testAChild>::allFunk;
	}

	virtual ~testAChild(){}

	void doFooC(std::list<dData> inData){ std::cout << me << " doing C" << std::endl; }
};

const funkHand testAChild::fooC = tMan<testAChild>::makeFunk("do foo C", &testAChild::doFooC);

void
runTest(){
	testA A;
	testAChild B;
	std::list<dData> data;

	tMan<testAChild>::extend<testA>(); // could be done as part of a draft.

	A.me = "parent";
	B.me = "child";

	A.jack(testA::fooA, data);
	B.jack(testA::fooA, data);
	A.jack(testA::fooB, data);
	B.jack(testAChild::fooC, data);
	try{ A.jack(testAChild::fooC, data); }catch(std::exception){ std::cout << "caught like a boss" << std::endl; }
}

}





int
main(int argc, char **argv){
	int result = EXIT_FAILURE;

	std::cout << "Running gamestool tests. Version 0.1" << std::endl;

	testZone::runTest();

	gt::gWorld.take( new gt::cWorld() );

#ifdef GTUT_GOOGLE
	::testing::InitGoogleTest(&argc, argv);
	result = RUN_ALL_TESTS();
#endif

	gt::gWorld.get()->makeProfileReport(std::cout);
	gt::gWorld.cleanup();

	return result;
}

