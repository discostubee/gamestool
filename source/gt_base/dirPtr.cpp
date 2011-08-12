#include "dirPtr.hpp"

////////////////////////////////////////////////////////////
// Tests
using namespace gt;

#ifdef GTUT

	//!\brief	test class designed to test if every instance was destroyed.
	class cAmIDeleted{
	public:
		static int count;
		cAmIDeleted(){ ++count; }
		~cAmIDeleted(){ --count; }
	};

	int cAmIDeleted::count = 0;

	typedef tDirPtr<cAmIDeleted> ptrTest;

	ptrTest foo(){
		static ptrTest singleton(new cAmIDeleted);
		return singleton;
	}

	GTUT_START(figmentPointer, Deletion){
		ptrTest A(new cAmIDeleted); // smart pointer to redirection pointer to instance.
		ptrTest B = A;				// should only copy the pointer to the redirection pointer.
		ptrTest C(new cAmIDeleted);	// a different redirector to a different.
		ptrTest D;

		GTUT_ASRT(A->count, "must be able to use overloaded pointer operators" );
		GTUT_ASRT((*B).count, "must be able to dereference" );
		GTUT_ASRT(A==B, "A isn't the same as B");
		GTUT_ASRT(A!=C, "A can't be the same as C");
		GTUT_ASRT(A!=D, "A can't be the same as D which is null");
		A.redirect(C);	// should clean the original instance. Both A and B should point to the same location, which is now C.

		// A, B and C should all have the same reference count now.
		GTUT_ASRT(A==C, "A didn't redirect to C");
		GTUT_ASRT(B==C, "B didn't redirect to C");
		GTUT_ASRT(C!=D, "C can't equal D");
		D = foo();
	}GTUT_END;

	GTUT_START(figmentPointer, Redirection){
		int startCount = cAmIDeleted::count;
		{
			ptrTest A(new cAmIDeleted); // smart pointer to redirection pointer to instance.
			ptrTest B = A;				// should only copy the pointer to the redirection pointer.
			ptrTest C(new cAmIDeleted);	// a different redirector to a different.
			ptrTest D;

			GTUT_ASRT(cAmIDeleted::count > startCount, "not counting each instance being made.");
			A.redirect(C);	// should clean the original instance. Both A and B should point to the same location, which is now C.
			GTUT_ASRT(cAmIDeleted::count-1 == startCount, "Instance not cleaned up when it was redirected.");
			D = foo();
		}
		GTUT_ASRT(startCount == cAmIDeleted::count, "Not every figment was deleted.");
	}GTUT_END;

	GTUT_START(figmentPointer, Change){

	}GTUT_END;

#endif
