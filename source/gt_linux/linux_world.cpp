#include "gt_base/context.hpp"
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

void
cLinuxWorld::openAddon(const dStr &name){
	dNameHash hash = makeHash(name.c_str());

	if(mLibHandles.find(hash) != mLibHandles.end()){
		DBUG_LO("addon " << name << " already open");
		return;
	}

	dStr libPath = "addon" + name;
	#if defined(DEBUG) && defined(GT_THREADS)
		libPath += "_dt";
	#elif defined(DEBUG)
		libPath += "_d";
	#elif defined(GT_THREADS)
		libPath += "_t";
	#endif
		libPath += ".so";

	DBUG_LO("using Linux to load shared library " << libPath);

	void *handLib = dlopen(libPath.c_str(), RTLD_LAZY);
	if(handLib != NULL){
		mLibHandles[hash] = handLib;

		draftFoo fn = reinterpret_cast<draftFoo>(dlsym(handLib, "draftAll"));

		if( fn != NULL ){
			(*fn)(gWorld.get().get());
		}else{
			WARN_S("Unable to use shared library function 'draftAll' because " << dlerror());
		}

	}else{
		WARN_S("unable to load shared library '" << libPath << "' because " << dlerror() );
	}
}

void
cLinuxWorld::closeAddon(const dStr &name){
	dNameHash hash = makeHash(name.c_str());
	mapNameToHandle::iterator found = mLibHandles.find(hash);

	if(found == mLibHandles.end()){
		WARN_S("Addon " << name << " wasn't open in the first place.");
		return;
	}

	if( found->second != NULL ){
		DBUG_LO("closing addon " << name);

		void (*fn)();
		fn = reinterpret_cast<void(*)()>(dlsym(found->second, "closeLib"));

		if( fn != NULL ){
			(*fn)();
		}else{
			DBUG_LO("unable to close shared library '" << name << "' because " << dlerror() );
		}
		dlclose(found->second);

	}else{
		WARN_S("Addon " << name << " handle was null.");
	}

	mLibHandles.erase(found);
}
