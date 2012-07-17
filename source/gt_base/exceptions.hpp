/*
 * !\file	exceptions.hpp
 * !\brief	Contains different comment exception types as well as some assertions.
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

#ifndef EXCEPTIONS_HPP
#define EXCEPTIONS_HPP

#include "gt_string.hpp"
#include <sstream>

namespace excep{

	//!\brief	Base type of all errors. Uses a stored string that is filled out on creation. These exceptions are not meant to be used to handle
	//!			memory exceptions, and so dynamic objects should still be fine to use for these exceptions.
    class base_error: public std::exception{
    private:
        dStr mInfo;

    public:
        base_error(const char* pFile, const unsigned int pLine) throw(){
			try{
				std::stringstream ss;
				ss << "Problem in file '" << pFile << "' at line " << pLine << ". Info: ";
				mInfo = ss.str();
			}catch(...){
			}
        }

		base_error(const char* pExtraInfo, const char* pFile, const unsigned int pLine){
			try{
				std::stringstream ss;
				ss << "Error in file '" << pFile << "' at line " << pLine << ". Info: " << pExtraInfo;
				mInfo = ss.str();
			}catch(...){
			}
		}

        virtual ~base_error() throw(){}

		virtual const char* what() const throw(){ return mInfo.data(); }

        void addInfo(const dStr &pInfo){ mInfo.append(pInfo); }

        template<typename T>
        dStr operator << (const T &pT) { dStr out; out + mInfo + pT; return out; }
    };

    //!\brief	Useful macro to make writing error bombs easier to write.
	#define THROW_BASEERROR(s) { std::stringstream ss; ss << s; throw excep::base_error(ss.str().c_str(), __FILE__, __LINE__); }

    //!\brief	Nobody knows.
	class unknownError: public base_error{
	public:
		unknownError(const char* pFile, const unsigned int pLine) throw():
            base_error(pFile, pLine)
        {
		    addInfo(dStr("Unknown error"));
		}
		virtual ~unknownError() throw(){}

	};

	//!\brief	Thrown when you try to find something and it isn't there.
	class notFound: public base_error{
	public:
		notFound(const char* pDidntFind, const char* pFile, const unsigned int pLine) throw():
            base_error(pFile, pLine)
        {
		    std::stringstream ss;
		    ss << "Didn't find " << pDidntFind;
		    addInfo(ss.str());
		}
		virtual ~notFound() throw(){}

	};

	//!\brief	Thrown when a NULL pointer is passed in, and it can't be handled nicely.
	class isNull: public base_error{
	public:
		isNull(const char* pFile, const unsigned int pLine) throw():
            base_error(pFile, pLine)
        {
            addInfo("Is Null");
		}
		virtual ~isNull() throw(){}
	};

	//!\brief	Generic error used when an interator or index is out of range.
	class outOfRange: public base_error{
	public:
		outOfRange(size_t maxRange, int index, const char* pFile, const unsigned int pLine) throw():
			base_error(pFile, pLine)
		{
			std::stringstream ss;
			ss << "Out of range. Range " << maxRange << ", index " << index;
			addInfo(ss.str());
		}
	};

	//!\brief	Handy if you want to have a class that is a mix of overloading and pure virtual. Put another way, if you want a class that can be
	//!			instantiated in it's base form, but some of it's functions are not meant to be used.
	class dontUseThis:  public base_error{
	public:
		dontUseThis(const char* pFile, const unsigned int pLine) throw():
            base_error(pFile, pLine)
        {
            addInfo("What are you doing! Don't use this.");
		}
		virtual ~dontUseThis() throw(){}
	};

	class underFlow: public excep::base_error{
	public:
		underFlow(const char* pFile, const unsigned int pLine) throw():
            base_error(pFile, pLine)
         { addInfo("buffer underflow"); }
		virtual ~underFlow() throw(){}
	};

	class overFlow: public excep::base_error{
	public:
		overFlow(const char* pFile, const unsigned int pLine) throw():
			base_error(pFile, pLine)
		 { addInfo("buffer overflow"); }
		virtual ~overFlow() throw(){}
	};

	//!\brief	Generic error trown when a function tries to copy something it should't (IE, polymorphed classes).
	class cantCopy: public base_error{
	public:
		cantCopy(const dStr &copyer, const dStr &copyee, const char* pFile, const unsigned int pLine) throw():
			base_error(pFile, pLine)
		{
			std::stringstream ss;
			ss << copyer << " can't copy " << copyee;
			addInfo(ss.str());
		}

		cantCopy(const char* copyer, const char* copyee, const char* pFile, const unsigned int pLine) throw():
			base_error(pFile, pLine)
		{
			std::stringstream ss;
			ss << copyer << " can't copy " << copyee;
			addInfo(ss.str());
		}

		virtual ~cantCopy() throw(){}

	};
}

#ifdef DEBUG
	#define ASRT_TRUE(p, s)			if(!(p)) throw ::excep::base_error(s, __FILE__, __LINE__)
	#define ASRT_NOTNULL(p) 		if(p == NULL) throw ::excep::isNull( __FILE__, __LINE__)
	#define ASRT_INRANGE(vec, idx)	if( idx < 0 || idx >= vec.size() ) throw ::excep::outOfRange(vec.size(), idx, __FILE__, __LINE__)
	#define ASRT_NOTSELF(p)			if( p == this) throw ::excep::base_error("Pointer is self", __FILE__, __LINE__)
	#define DONT_USE_THIS			throw excep::dontUseThis(__FILE__, __LINE__)
#else
	#define ASRT_TRUE(p, s)
	#define ASRT_NOTNULL(p)
	#define ASRT_INRANGE(vec, ite)
	#define ASRT_NOTSELF(p)
	#define DONT_USE_THIS
#endif

#define NOTSELF(p)			if(p == this) return *this

#endif
