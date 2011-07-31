//!\file	main.cpp
//!\brief	

// Include the stuff we want to test.
#include <gt_base/figment.hpp>


/*
class testMe;

class postFace{
public:
	const dNameHash ID;

	postFace(dNameHash inID) : ID(inID) {}
	virtual ~postFace() {}

	virtual void say(testMe *pIn) {}
};

template <typename T>
class post : public postFace{
public:
	typedef void (T::*fooPtr)();

	post( fooPtr inFoo, dNameHash inID ) : postFace(inID), myPtr(inFoo) {}
	virtual ~post() {}
	virtual void say(testMe *pIn);

private:
	fooPtr myPtr;

	static void callFoo(T *pIn, fooPtr use) { (pIn->*use)(); }
};

class testMe{
public:
	typedef std::set<dNameHash> hasFoo;

	static postFace *postHi;
	static postFace *postBye;

	std::string myStr;

	testMe(char const * inStr) : myStr(inStr), realFoo(&myFoo) { }
	virtual ~testMe() {}

	void saySomething(postFace *inPost) {
		if( realFoo->find(inPost->ID) != realFoo->end() ) inPost->say(this);
	}

	template <typename T> static postFace* makePost(post<T>::footPtr inPtr) {
		dNameHash fooNH = makeHash( (dNatChar*)typeid(post<T>).name() );
		postFace* postPtr = new post<T>(fooPtr, fooNH);

		T::myFoo.insert(fooNH);
		return postPtr;
 	}

protected:
	void sayHi() { std::cout << myStr << " says hi" << std::endl; }
	void sayBye() { std::cout << myStr << " says bye" << std::endl; }

	hasFoo* realFoo;

private:
	static hasFoo myFoo;

};

postFace *testMe::postHi = testMe::makePost(&testMe::sayHi);
postFace *testMe::postBye = testMe::makePost(&testMe::sayBye);
testMe::hasFoo testMe::myFoo;

template<typename T>
void post<T>::say(testMe *pIn) { callFoo(dynamic_cast<T*>(pIn), myPtr); }

class testMoo : public testMe{
public:
	static postFace *postMoo;

	testMoo(char const * inStr) : testMe(inStr) { realFoo = &myFoo; }
	virtual ~testMoo() {}

protected:
	void sayMoo() { std::cout << myStr << " says moo"  << std::endl; }

private:
	static hasFoo myFoo;
};

testMe::hasFoo testMoo::myFoo;
postFace *testMoo::postMoo = testMe::makePost(&testMoo::sayMoo);

void testAll(){
	testMe A("A");
	testMoo B("B");
	A.saySomething(A.postHi);
	B.saySomething(A.postHi);
	A.saySomething(B.postMoo);
	B.saySomething(B.postMoo);
}
*/

int
main(int argc, char **argv){
	int result = EXIT_FAILURE;

	std::cout << "Running gamestool tests. Version 0.1" << std::endl;

	//testAll();

	//demo::threadTestFoo();

	gt::gWorld.take( new gt::cWorld() );

#ifdef GTUT_GOOGLE
	::testing::InitGoogleTest(&argc, argv);
	result = RUN_ALL_TESTS();
#endif

	gt::gWorld.get()->makeProfileReport(std::cout);
	gt::gWorld.cleanup();

	return result;
}

