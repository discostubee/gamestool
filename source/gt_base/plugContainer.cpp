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

////////////////////////////////////////////////////////////
using namespace gt;
cBasePlugContainer::cBasePlugContainer(){

}

cBasePlugContainer::~cBasePlugContainer(){
}

cBase_plug::dPlugType
cBasePlugContainer::getType() const{
	return cBase_plug::genPlugType<cBasePlugContainer>();
}

////////////////////////////////////////////////////////////
// Tests
#ifdef GTUT

GTUT_START(testPlugLinier, assignAppend){
	tPlugLinearContainer<int, std::vector> testArray;
	tPlugLinearContainer<int, std::list> testList;
	std::vector<int> a;
	std::list<int> b;

	a.push_back(1);
	a.push_back(2);
	a.push_back(3);
	testArray = a;
	//testList = a;		//- can't copy different containers directly

	b.push_back(4);
	b.push_back(5);
	b.push_back(6);
	//testArray += b;	//- can't copy different containers directly
	testList += b;

	testArray += testList;

}GTUT_END;

GTUT_START(testPlugLinier, saveLoad){

}GTUT_END;




#endif
