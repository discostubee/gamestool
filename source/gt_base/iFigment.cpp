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

#include "figment.hpp"

using namespace gt;


////////////////////////////////////////////////////////////

ptrFig::ptrFig() :
	tDirPtr<iFigment>()
{}

ptrFig::ptrFig(iFigment* pFig):
	tDirPtr<iFigment>(pFig)
{}

ptrFig::ptrFig(const ptrFig &pPtr) :
	tDirPtr<iFigment>(pPtr)
{}

ptrFig::~ptrFig(){
}

ptrFig&
ptrFig::operator=(ptrFig const &pPtr){
	if(!pPtr.mDir)
		return *this;

	if(mDir == pPtr.mDir)	// Should also handle self reference.
		return *this;

	if(mDir){
		if(unique())
			delete mDir;
		else
			mDir->unlink();
	}

	mDir = pPtr.mDir;
	mDir->link();

	return *this;
}

bool
ptrFig::operator==(ptrFig const &pPtr) const{
	if(mDir==NULL){
		if(pPtr.mDir==NULL) return true; else return false;
	}else if(pPtr.mDir==NULL){
		return false;
	}
	return mDir->get() == pPtr.mDir->get();
}

bool
ptrFig::operator!=(ptrFig const &pPtr) const{
	if(mDir==NULL){
		if(pPtr.mDir==NULL) return false; else return true;
	}else if(pPtr.mDir==NULL){
		return true;
	}
	return mDir->get() != pPtr.mDir->get();
}

void
ptrFig::linkDir(tDirector<iFigment> *aDirector){
	if(mDir == NULL){
		mDir = aDirector;
		mDir->link();
	}
}

tDirector<iFigment> *ptrFig::getDir(){
	return mDir;
}

