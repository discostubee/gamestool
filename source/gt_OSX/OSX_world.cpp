#include "OSX_world.hpp"

////////////////////////////////////////////////////////////
using namespace gt;

timeval cOSXWorld::tempTime;

dMillisec
cOSXWorld::getOSXTime(){
	gettimeofday(&tempTime, NULL);

	return tempTime.tv_usec;
}

cOSXWorld::cOSXWorld(){
	mProfiler->mGetTime = &getOSXTime;
}

cOSXWorld::~cOSXWorld(){
	mRoot.reset();	//so that our console displays that all figments were destroyed.
	flushLines();
}

dMillisec
cOSXWorld::getAppTime(){
	return getOSXTime();
}

void
cOSXWorld::loop(){
	//!\todo get the main OSX program loop going.
	cContext worldContext;

	DBUG_LO("OSX world loop");
	while(mKeepLooping){
		mRoot->run(&worldContext);
		flushLines();
	}
}

void
cOSXWorld::flushLines(){
	std::cout << "count " << mLines->size() << std::endl; //!!! test code.º
	for(dLines::iterator i = mLines->begin(); i != mLines->end(); ++i){
		std::cout << (*i) << std::endl; //!!!Put back in.
	}
	mLines->clear();
}
