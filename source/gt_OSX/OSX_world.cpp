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
	try{
		for(std::list<dStr>::iterator itr = mAddonsToClose.begin(); itr != mAddonsToClose.end(); ++itr)
			closeAddon(*itr);

		mAddonsToClose.clear();
		mRoot.redirect(NULL);	//so that our console displays that all figments were destroyed.
		flushLines();
	}catch(...){
	}
}

dMillisec
cOSXWorld::getAppTime(){
	return getOSXTime();
}

void
cOSXWorld::loop(){
	cContext worldContext;
	std::list<dStr>::iterator itrAddonsToClose;

	DBUG_LO("OSX world loop");
	while(mKeepLooping){
		mRoot->run(&worldContext);

		for(itrAddonsToClose = mAddonsToClose.begin(); itrAddonsToClose != mAddonsToClose.end(); ++itrAddonsToClose)
			closeAddon(*itrAddonsToClose);
		mAddonsToClose.clear();

		flushLines();
	}
}

void
cOSXWorld::flushLines(){
	mLines->clear();
}

void
cOSXWorld::openAddon(const dStr &name){
	dNameHash hash = makeHash(name.c_str());

	if(mLibHandles.find(hash) != mLibHandles.end()){
		DBUG_LO("addon " << name << " already open");
		return;
	}

	dStr libPath = "addon" + name;
	#if defined(DEBUG) && defined(GT_THREADS)
		libPath += "_dt.dylib";
	#elif defined(DEBUG)
		libPath += "_d.dylib";
	#elif defined(GT_THREADS)
		libPath += "_t.dylib";
	#else
		libPath += ".dylib";
	#endif

	DBUG_LO("using OSX to load shared library " << name << ", with path " << libPath);

	void *handLib = dlopen(libPath.c_str(), RTLD_LAZY);
	if(handLib != NULL){
		mLibHandles[hash] = handLib;

		draftFoo fn = reinterpret_cast<draftFoo>(dlsym(handLib, "draftAll"));

		if( fn != NULL ){
			(*fn)(gWorld.get().get());
			mAddonsToClose.push_back(name);
		}else{
			WARN_S("Unable to use shared library function 'draftAll' because " << dlerror());
		}

	}else{
		WARN_S("unable to load shared library '" << libPath << "' because " << dlerror() );
	}
}

void
cOSXWorld::closeAddon(const dStr &name){
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
