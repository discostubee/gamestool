#include "terminal_world.hpp"

////////////////////////////////////////////////////////////
using namespace gt;


cTerminalWorld::cTerminalWorld(){
}

cTerminalWorld::~cTerminalWorld(){
	try{
		flushLines();
	}catch(...){
	}
}

void
cTerminalWorld::flushLines(){
#	ifdef GT_THREADS
		boost::lock_guard<boost::recursive_mutex> lock(*mLineGuard);
#	endif

	for(dLines::iterator i = mLines->begin(); i != mLines->end(); ++i){
		std::cout << (*i) << std::endl;
	}
	mLines->clear();
}
