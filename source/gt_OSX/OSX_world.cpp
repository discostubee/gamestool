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
	mProfiles->mGetTime = &getOSXTime;
}

cOSXWorld::~cOSXWorld(){
	mRoot.redirect(NULL);	//so that our console displays that all figments were destroyed.
	flushLines();
}

dMillisec
cOSXWorld::getAppTime(){
	return getOSXTime();
}

void
cOSXWorld::loop(){
	cContext worldContext;

	DBUG_LO("OSX world loop");
	while(mKeepLooping){
		mRoot->run(&worldContext);
		flushLines();
	}
}

void
cOSXWorld::flushLines(){
	//std::cout << "flush from "<< mLines << ", "<< mLines->size() <<" lines" << std::endl;
	for(dLines::iterator i = mLines->begin(); i != mLines->end(); ++i){
		std::cout << (*i) << std::endl;
	}
	mLines->clear();
}
