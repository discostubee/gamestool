#include "linux_world.hpp"

////////////////////////////////////////////////////////////
using namespace gt;

timeval cLinuxWorld::tempTime;

dMillisec
cLinuxWorld::getLinuxTime(){
	gettimeofday(&tempTime, NULL);

	return tempTime.tv_usec;
}

cLinuxWorld::cLinuxWorld(){
	mProfiles->mGetTime = &getLinuxTime;
}

cLinuxWorld::~cLinuxWorld(){
	mRoot.redirect(NULL);	//so that our console displays that all figments were destroyed.
	flushLines();
}

dMillisec
cLinuxWorld::getAppTime(){
	return getLinuxTime();
}

void
cLinuxWorld::loop(){
	//!\todo get the main linux program loop going.
	cContext worldContext;

	DBUG_LO("linux world loop");
	while(mKeepLooping){
		mRoot->run(&worldContext);
		flushLines();
	}
}

void
cLinuxWorld::flushLines(){
	for(dLines::iterator i = mLines->begin(); i != mLines->end(); ++i){
		std::cout << (*i) << std::endl; //!!!Put back in.
	}
	mLines->clear();
}
