#include "terminal_world.hpp"

////////////////////////////////////////////////////////////
using namespace gt;


cTerminalWorld::cTerminalWorld(){
}

cTerminalWorld::~cTerminalWorld(){
	try{
		for(std::list<dStr>::iterator itr = mAddonsToClose.begin(); itr != mAddonsToClose.end(); ++itr)
			closeAddon(*itr);

		mAddonsToClose.clear();
		mRoot.redirect(NULL);	//so that our console displays that all figments were destroyed.
		flushLines();
	}catch(...){
	}
}

void
cTerminalWorld::flushLines(){
#	ifdef GT_THREADS
		boost::lock_guard<boost::recursive_mutex> lock(*xLineGuard);
#	endif

	for(dLines::iterator i = mLines->begin(); i != mLines->end(); ++i){
		std::cout << (*i) << std::endl;
	}
	mLines->clear();
}
