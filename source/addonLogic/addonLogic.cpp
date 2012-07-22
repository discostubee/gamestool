#include "addonLogic.hpp"

DYN_LIB_DEF(void)
draftAll(gt::cWorld* pWorld){
	try{
		gt::redirectWorld(pWorld);

		gt::tOutline<gt::cLogicInput>::draft();
		gt::tOutline<gt::cLogicOutput>::draft();
		gt::tOutline<gt::cArithmetic>::draft();
		gt::tOutline<gt::cBoolLogic>::draft();
		gt::tOutline<gt::cEquation>::draft();
		DBUG_LO("logic addon open.");
	}catch(excep::base_error &e){
		WARN(e);

	}catch(...){
		std::cout << "unknown error while opening logic addon" << std::endl;
	}
}

DYN_LIB_DEF(void)
closeLib(){
	try{
		gt::tOutline<gt::cLogicInput>::removeFromWorld();
		gt::tOutline<gt::cLogicOutput>::removeFromWorld();
		gt::tOutline<gt::cArithmetic>::removeFromWorld();
		gt::tOutline<gt::cBoolLogic>::removeFromWorld();
		gt::tOutline<gt::cEquation>::removeFromWorld();

		cTracker::makeReport(std::cout);

		DBUG_LO("closed logic addon.");
		gt::gWorld->flushLines();
	}catch(excep::base_error &e){
		WARN(e);

	}catch(...){
		std::cout << "unknown error while closing logic addon" << std::endl;
	}
}
