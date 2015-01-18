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


#include "opOnAny.hpp"



////////////////////////////////////////////////////////////
using namespace gt;

cAnyOp::cAnyOp(){

}

cAnyOp::~cAnyOp(){
	try{
		demerge();

	//- Don't do this. There's no reason for it because it's a singleton, and it's dangerous.
	//		for(dKats::iterator itrK = mKats.begin(); itrK != mKats.end(); ++itrK){
	//			itrK->second->unlink(this);
	//		}
	}catch(...){

	}
}

void
cAnyOp::merge(cAnyOp * pOther){

}

void
cAnyOp::demerge(){
	for(std::list<cAnyOp*>::iterator itrL = mLinks.begin(); itrL != mLinks.end(); ++itrL){
		for(dKats::iterator itrK = (*itrL)->mKats.begin(); itrK != (*itrL)->mKats.end(); ++itrK){
			itrK->second->unlink(this);
		}
	}
	mLinks.clear();
}

cAnyOp&
cAnyOp::getRef(){
	static cAnyOp xOps;
	return xOps;
}



