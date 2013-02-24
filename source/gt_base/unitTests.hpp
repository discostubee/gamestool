/*
**********************************************************************************************************
 *  Copyright (C) 2010  Stuart Bridgens
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License (version 3) as published by
 *  the Free Software Foundation.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *********************************************************************************************************
*/

////////////////////////////////////////////////////////////////////////////////////////
// Config

//- Enable unit tests by putting this option into your compiler settings.
//#define GTUT

//- Google testing framework enabled. Put this option into your compiler settings.
//#define GTUT_GOOGLE

//- Enable below if you wish to avoid any artificial slowdown normally used to test threading.
//#define GTUT_SPEEDTEST

////////////////////////////////////////////////////////////////////////////////////////
//

#ifndef UNITTEST_HPP
#	define UNITTEST_HPP
#	ifdef DEBUG

#		ifdef GTUT_GOOGLE
#			include <gtest/gtest.h>	// You'll need to point your linker to the location you downloaded gtest to.

// These are the macros every framework needs to implement.

			//!\brief	If this statment is false, then it fails and displays a message as to why.
#			define GTUT_ASRT(statement, failMsg)\
				ASSERT_TRUE(statement) << std::endl << "Problem: " << failMsg

			//!\brief	Try calling a function, reports if anything is thrown out of it.
#			define TRYME(f)\
				try{ f; }\
				catch(std::exception &e){ ASSERT_TRUE(false) << std::endl << "Problem: " << e.what(); }\
				catch(...){ ASSERT_TRUE(false) << "Unknown exception"; }

#			define GTUT_START(name, args)\
				TEST(name, args){\
					try

#			define GTUT_END\
					catch(excep::base_error &e){ ASSERT_TRUE(false) << std::endl << "Problem: " << e.what(); }\
					catch(...){ ASSERT_TRUE(false) << "Unknown exception"; }\
				}

#		endif
#	endif
#endif
