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

template<typename T>
dStr
base_error::operator << (const T &pT) {
	dStr out;
	out + mInfo + pT;
	return out;
}

///////////////////////////////////////////////////////////////////////
using namespace excep;

unknownError::unknownError(const char* pFile, const unsigned int pLine) throw()
:	base_error(pFile, pLine)
{
	addInfo(dStr("Unknown error"));
}

unknownError::~unknownError() throw(){
}

///////////////////////////////////////////////////////////////////////
using namespace excep;

notFound::notFound(const char* pDidntFind, const char* pFile, const unsigned int pLine) throw()
:	base_error(pFile, pLine)
{
	std::stringstream ss;
	ss << "Didn't find " << pDidntFind;
	addInfo(ss.str());
}

notFound::~notFound() throw(){
}

///////////////////////////////////////////////////////////////////////
using namespace excep;

isNull::isNull(const char* pFile, const unsigned int pLine) throw()
:	base_error(pFile, pLine)
{
	addInfo("Is Null");
}

isNull::~isNull() throw(){
}

///////////////////////////////////////////////////////////////////////
using namespace excep;

outOfRange::outOfRange(size_t maxRange, int index, const char* pFile, const unsigned int pLine) throw()
:	base_error(pFile, pLine)
{
	std::stringstream ss;
	ss << "Out of range. Range " << maxRange << ", index " << index;
	addInfo(ss.str());
}

outOfRange::~outOfRange() throw(){
}

///////////////////////////////////////////////////////////////////////
using namespace excep;

dontUseThis::dontUseThis(const char* pFile, const unsigned int pLine) throw()
:	base_error(pFile, pLine)
{
	addInfo("What are you doing! Don't use this.");
}

dontUseThis::~dontUseThis() throw(){
}

///////////////////////////////////////////////////////////////////////
using namespace excep;

underFlow::underFlow(const char* pFile, const unsigned int pLine) throw()
:	base_error(pFile, pLine)
{
	addInfo("buffer underflow");
}

underFlow::~underFlow() throw(){
}

///////////////////////////////////////////////////////////////////////
using namespace excep;

overFlow::overFlow(const char* pFile, const unsigned int pLine) throw()
:	base_error(pFile, pLine)
{
	addInfo("buffer overflow");
}

overFlow::~overFlow() throw(){
}


///////////////////////////////////////////////////////////////////////
using namespace excep;

cantCopy::cantCopy(const dStr &copyer, const dStr &copyee, const char* pFile, const unsigned int pLine) throw()
:	base_error(pFile, pLine)
{
	std::stringstream ss;
	ss << copyer << " can't copy " << copyee;
	addInfo(ss.str());
}

cantCopy::cantCopy(const char* copyer, const char* copyee, const char* pFile, const unsigned int pLine) throw()
:	base_error(pFile, pLine)
{
	std::stringstream ss;
	ss << copyer << " can't copy " << copyee;
	addInfo(ss.str());
}

cantCopy::~cantCopy() throw(){
}

///////////////////////////////////////////////////////////////////////
using namespace excep;

badParameter::badParameter(const dStr &pNamePar, const char * pFile, const unsigned int pLine) throw()
: base_error(pFile, pLine)
{
	addInfo("Bad parameter");
	addInfo(pNamePar);
}

badParameter::~badParameter() throw(){
}

///////////////////////////////////////////////////////////////////////
using namespace excep;

char delayExcep::xBuff[512];
size_t delayExcep::xPos;

delayExcep::~delayExcep() throw(){
}

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

const char* delayExcep::what() const throw(){
	return mMsg.c_str();
}

delayExcep::delayExcep(const char *msg):
	mMsg(msg)
{}
