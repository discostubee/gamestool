
#ifndef CONTEXT_HPP
#define CONTEXT_HPP

#include "world.hpp"

#include <deque>

///////////////////////////////////////////////////////////////////////////////////
// typedefs and defines
namespace gt{
	typedef std::deque<iFigment*> dProgramStack;
	typedef std::map<iFigment*, int> dFigSigCount;
	typedef std::map<dNameHash, dProgramStack> dPancakes;	//!< There are many different kinds of pancakes, and each plate can have any number of that kind on it.
}

///////////////////////////////////////////////////////////////////////////////////
// classes
namespace gt{

//-------------------------------------------------------------------------------------
//!\class	cContext
//!\brief	A context is used to avoid circular references and to also create a way to
//!			block threads from running into each other.
class cContext{
public:
	cContext();
	cContext(const cContext & copyMe);
	~cContext();

	void add(iFigment* pFig);
	void finished(iFigment* pFig);
	bool isStacked(iFigment* pFig);
	refFig getLastOfType(dNameHash pType);

private:
	dProgramStack mStack;
	dFigSigCount mTimesStacked;	//!<
	dPancakes mPlateOPancakes;	//!< Maps types to a stack, so you can tell what the most recent object of a certain type is.

	dFigSigCount::iterator figSigItr;
	dPancakes::iterator cakeItr;
};

}

///////////////////////////////////////////////////////////////////////////////////
// Helper macros
#define CON_START(con)	if(con->isStacked(this)){ return; } con->add(this)
#define CON_STOP(con)	con->finished(this);

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
