
#ifndef CONTEXT_HPP
#define CONTEXT_HPP

#include "blueprint.hpp"

#include <stack>
#include <map>

///////////////////////////////////////////////////////////////////////////////////
// typedefs and defines
namespace gt{
	typedef std::deque<cFigment*> dProgramStack;
	typedef std::map<cFigment*, int> dFigSigCount;
	typedef std::map<dNameHash, dProgramStack> dPancakes;	//!< There are many different kinds of pancakes, and each plate can have any number of that kind on it.
}

///////////////////////////////////////////////////////////////////////////////////
// classes
namespace gt{

//-------------------------------------------------------------------------------------
//!\class	cContext
//!\brief	A context is used to pass information to a figments that is downstream, about
//!			a figment upstream.
class cContext{
private:
	dProgramStack mStack;
	dFigSigCount mTimesStacked;	//!<
	dPancakes mPlateOPancakes;	//!< Maps types to a stack, so you can tell what the most recent object of a certain type is.

	dFigSigCount::iterator figSigItr;
	dPancakes::iterator cakeItr;

public:
	cContext();
	~cContext();

	void add(cFigment* pFig);
	void finished(cFigment* pFig);
	bool isStacked(cFigment* pFig);
	cFigment* getLastOfType(dNameHash pType);
};

}

///////////////////////////////////////////////////////////////////////////////////
// errors
namespace excep{
	class stackFault: public base_error{
	public:
		stackFault(
			gt::dProgramStack &pBadStack,
			const std::string &pMoreInfo,
			const char* pFunc,
			const unsigned int pLine
		): base_error(pFunc, pLine){
			addInfo( dStr("stack fault: ") + pMoreInfo );
		}
		virtual ~stackFault() throw() {}
	};

}


#endif
