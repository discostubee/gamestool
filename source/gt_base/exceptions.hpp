/*
 * !\file	exceptions.hpp
 * !\brief	Contains different comment exception types as well as some assertions.
 *
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
        base_error(const dNatChar* pFile, const unsigned int pLine) throw(){
			try{
				std::stringstream ss;
				ss << "Problem in file '" << pFile << "' at line " << pLine << ". Info: ";
				mInfo = ss.str();
			}catch(...){
			}
        }

		base_error(const dNatChar* pExtraInfo, const dNatChar* pFile, const unsigned int pLine){
			try{
				std::stringstream ss;
				ss << "Error in file '" << pFile << "' at line " << pLine << ". Info: " << pExtraInfo;
				mInfo = ss.str();
			}catch(...){
			}
		}

        virtual ~base_error() throw(){}

		virtual const dNatChar* what() const throw(){ return mInfo.data(); }

        void addInfo(const dStr &pInfo){ mInfo.append(pInfo); }

        template<typename T>
        dStr operator << (const T &pT) { dStr out; out + mInfo + pT; return out; }
    };

    //!\brief	Nobody knows.
	class unknownError: public base_error{
	public:
		unknownError(const dNatChar* pFunc, const unsigned int pLine) throw():
            base_error(pFunc, pLine)
        {
		    addInfo(dStr("Unknown error"));
		}
		virtual ~unknownError() throw(){}

	};

	//!\brief	Thrown when you try to find something and it isn't there.
	class notFound: public base_error{
	public:
		notFound(const dNatChar* pDidntFind, const dNatChar* pFunc, const unsigned int pLine) throw():
            base_error(pFunc, pLine)
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
		isNull(const dNatChar* pFunc, const unsigned int pLine) throw():
            base_error(pFunc, pLine)
        {
            addInfo("Is Null");
		}
		virtual ~isNull() throw(){}
	};

	//!\brief	Generic error used when an interator or index is out of range.
	class outOfRange: public base_error{
	public:
		outOfRange(size_t maxRange, int index, const dNatChar* pFunc, const unsigned int pLine) throw():
			base_error(pFunc, pLine)
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
		dontUseThis(const dNatChar* pFunc, const unsigned int pLine) throw():
            base_error(pFunc, pLine)
        {
            addInfo("What are you doing! Don't use this.");
		}
		virtual ~dontUseThis() throw(){}
	};

	//!\brief	Generic error trown when a function tries to copy something it should't (IE, polymorphed classes).
	class cantCopy: public base_error{
	public:
		cantCopy(const dStr &copyer, const dStr &copyee, const dNatChar* pFunc, const unsigned int pLine) throw():
			base_error(pFunc, pLine)
		{
			std::stringstream ss;
			ss << copyer << " can't copy " << copyee;
			addInfo(ss.str());
		}

		cantCopy(const dNatChar* copyer, const dNatChar* copyee, const dNatChar* pFunc, const unsigned int pLine) throw():
			base_error(pFunc, pLine)
		{
			std::stringstream ss;
			ss << copyer << " can't copy " << copyee;
			addInfo(ss.str());
		}

		virtual ~cantCopy() throw(){}

	};
}

#ifdef DEBUG
	#define ASRT_NOTNULL(p) 		if(p == NULL) throw ::excep::isNull( __FILE__, __LINE__)
	#define ASRT_INRANGE(vec, ite)	if( ite < 0 || ite >= vec.size() ) throw ::excep::outOfRange(vec.size(), ite, __FILE__, __LINE__)
	#define ASRT_NOTSELF(p)			if( p == this) throw ::excep::base_error("Pointer is self", __FILE__, __LINE__)
	#define DONT_USE_THIS			throw excep::dontUseThis(__FILE__, __LINE__)
#else
	#define ASRT_NOTNULL(p)
	#define ASRT_INRANGE(vec, ite)
	#define ASRT_NOTSELF(p)
	#define DONT_USE_THIS
#endif

#define NOTSELF(p)			if(p == this) return *this

#endif
