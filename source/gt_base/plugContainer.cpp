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

#include "plugContainer.hpp"

using namespace gt;

////////////////////////////////////////////////////////////
// Tests
#ifdef GTUT

GTUT_START(testPlugArray, STLToPContainer){
	tPlugArray<int> testMe;
	std::vector<int> a;
	std::list<int> b;

	testMe = a;
	testMe = b;
}GTUT_END;

GTUT_START(testPlugList, STLToPContainer){
	tPlugList<int> testMe;
	std::vector<int> a;
	std::list<int> b;

	testMe = a;
	testMe = b;
}GTUT_END;

GTUT_START(testPlugArray, saveLoad){
	const size_t num = 5;

	cByteBuffer buff;
	{
		tPlugArray<int> A;
		for(size_t i=0; i < num; ++i)
			A.mContainer.push_back(i);

		A.save(&buff);
	}
	{
		tPlugArray<int> B;

		B.loadEat(&buff, NULL);
		for(int i=0; i < static_cast<int>(num); ++i)
			GTUT_ASRT(B.mContainer.at(i).get()==i, "not the right number");
	}

}GTUT_END;

GTUT_START(testPlugList, saveLoad){
	const size_t num = 5;

	cByteBuffer buff;
	{
		tPlugList<int> A;
		for(size_t i=0; i < num; ++i)
			A.mContainer.push_back(i);

		A.save(&buff);
	}
	{
		tPlugList<int> B;

		B.loadEat(&buff, NULL);
		//for(int i=0; i < static_cast<int>(num); ++i)
		//	GTUT_ASRT(B.mContainer.at(i).get() == i, "not the right number");
	}

}GTUT_END;

GTUT_START(testPlugMap, saveLoad){
	const short num = 5;

	cByteBuffer buff;
	{
		tPlugMap< int, short > A;
		for(short i=0; i < num; ++i)
			A.mContainer[i] = i * 2;

		A.save(&buff);
	}
	{
		tPlugMap< int, short > B;

		B.loadEat(&buff, NULL);
		for(short i=0; i < num; ++i)
			GTUT_ASRT(B.mContainer[i].get() == i * 2, "not the right number");
	}

}GTUT_END;



#endif
