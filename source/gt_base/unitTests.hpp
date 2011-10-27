
#ifndef UNITTEST_HPP
#define UNITTEST_HPP

	#ifdef DEBUG
		//#define GTUT //- Enable unit tests by putting this option into your compiler settings.

		#ifdef GTUT

			//- Choose a testing framework
			//#define GTUT_GOOGLE	//!< Google testing framework enabled. Put this option into your compiler settings.

			//!brief	Enable below if you wish to avoid any artificial slowdown normally used to test threading.
			//#define GTUT_SPEEDTEST

		#endif

		#ifdef GTUT_GOOGLE

			#include <gtest/gtest.h>	// You'll need to point your linker to the location you downloaded gtest to.

			// These are the macros every framework needs to implement.

			//!\brief	If this statment is false, then it fails and displays a message as to why.
			#define GTUT_ASRT(statement, failMsg) ASSERT_TRUE(statement) << std::endl << "Problem: " << failMsg

			//!\brief	Try calling a function, reports if anything is thrown out of it.
			#define TRYME(f) try{ f; }catch(std::exception &e){ ASSERT_TRUE(false) << e.what(); }catch(...){ ASSERT_TRUE(false) << "Unknown exception"; }

			#define GTUT_START(name, args)	TEST(name, args){ try

			#define GTUT_END catch(excep::base_error &e){ GTUT_ASRT(false, e.what()); }  }

		#else

			#define GTUT_START(t)
			#define GTUT_END

		#endif

	#endif

#endif
