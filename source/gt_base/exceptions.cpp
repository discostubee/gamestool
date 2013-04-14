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

using namespace excep;

char logExcep::xBuff[512];
size_t logExcep::xPos;

void logExcep::add(const char *msg){
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

void logExcep::shake(){
	CRITSEC;

	if(xPos != 0){
		logExcep e(xBuff);
		clearAll();
		throw e;
	}
}

void logExcep::clearAll(){
	memset(xBuff, 0, sizeof(xBuff));
	xPos = 0;
}

logExcep::~logExcep() throw(){
}

const char* logExcep::what() const throw(){
	return mMsg.c_str();
}

logExcep::logExcep(const char *msg):
	mMsg(msg)
{}