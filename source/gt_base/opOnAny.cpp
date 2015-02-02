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
	mOrig.insert(pOther);
	merge(&mKats, &pOther->mKats, this);
	merge(&pOther->mKats, &mKats, pOther);
}

void
cAnyOp::merge(dMapKatTypes *myKats, dMapKatTypes *yourKats, cAnyOp *me){
	dMapKatTypes::iterator itrK;
	dMapKatTypes::iterator found;

	for(itrK = myKats->begin(); itrK != myKats->end(); ++itrK){
		found = yourKats->find(itrK->first);
		if(found == yourKats->end()){
			found = yourKats->insert(
				yourKats->end(),
				dMapKatTypes::value_type(
					itrK->first,
					sKatOrig(
						itrK->second.mKat,
						me
					)
				)
			);
		}

		found->second.mKat->link(me, itrK->second.mKat);
	}
}

void
cAnyOp::demerge(){
	dMapKatTypes::iterator itrK;
	for(
		dSetOrig::iterator itrO = mOrig.begin();
		itrO != mOrig.end();
		++itrO
	){
		dMapKatTypes::iterator itrK = (*itrO)->mKats.begin();
		while(itrK != (*itrO)->mKats.end()){
			if(itrK->second.mOrig == this){
				dMapKatTypes::iterator eraseMe = itrK;
				++itrK;
				(*itrO)->mKats.erase(eraseMe);
			}else{
				itrK->second.mKat->unlink(this);
				++itrK;
			}
		}
	}
}

cAnyOp&
cAnyOp::getRef(){
	static cAnyOp xOps;
	return xOps;
}

