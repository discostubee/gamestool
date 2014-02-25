/*
 * !\file	exceptions.cpp
 * !\brief
 *
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

#include "exceptions.hpp"
#include "threadTools.hpp"

///////////////////////////////////////////////////////////////////////
using namespace excep;

fatal_error::fatal_error(const char* pFile, const unsigned int pLine) throw(){
	try{
		std::stringstream ss;
		ss << "Fatal error in file '" << pFile << "' at line " << pLine;
	}catch(...){

	}
}

fatal_error::~fatal_error() throw(){
}

const char*
fatal_error::what() const throw(){
	try{
		return mInfo.data();
	}catch(...){
		return "";
	}
}

///////////////////////////////////////////////////////////////////////
using namespace excep;

base_error::base_error(const char* pFile, const unsigned int pLine) throw(){
	try{
		std::stringstream ss;
		ss << "Problem in file '" << pFile << "' at line " << pLine << ". Info: ";
		mInfo = ss.str();
	}catch(...){
	}
}

base_error::base_error(const char* pExtraInfo, const char* pFile, const unsigned int pLine) throw(){
	try{
		std::stringstream ss;
		ss << "Error in file '" << pFile << "' at line " << pLine << ". Info: " << pExtraInfo;
		mInfo = ss.str();
	}catch(...){
	}
}

base_error::~base_error() throw(){
}

const char*
base_error::what() const throw(){
	try{
		return mInfo.data();
	}catch(...){
		return "";
	}
}

void
base_error::addInfo(const dStr &pInfo){
	mInfo.append(pInfo);
}

///////////////////////////////////////////////////////////////////////
using namespace excep;

char delayExcep::xBuff[512];
size_t delayExcep::xPos;

void delayExcep::add(const char *msg){
	CRITSEC;

	static bool setup = false;

	if(!setup)
		clearAll();

	if(msg != NULL){
		size_t l = strlen(msg);

		if(xPos + 2 + l > SIZE_BUFF && xPos < SIZE_BUFF -2)
			l -= SIZE_BUFF - 2 - xPos;

		if(l > 0){
			xBuff[xPos] = '\n';
			memcpy(&xBuff[xPos+1], msg, sizeof(char) * l);
			xPos += l +1;
		}
	}
}

void delayExcep::shake(){
	CRITSEC;

	if(xPos != 0){
		delayExcep e(xBuff);
		clearAll();
		throw e;
	}
}

void delayExcep::clearAll(){
	memset(xBuff, 0, sizeof(xBuff));
	xPos = 0;
}

delayExcep::~delayExcep() throw(){
}

const char* delayExcep::what() const throw(){
	return mMsg.c_str();
}

delayExcep::delayExcep(const char *msg):
	mMsg(msg)
{}
