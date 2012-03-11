#include "OSX_addon.hpp"

using namespace gt;

void
cAddon_OSX::draftAddon(const std::string &pName){
	if( mLibHandle == NULL ){
		#if defined(DEBUG) && defined(GT_THREADS)
			mLibPath = "addon" + pName + "_dt.dylib";
		#elif defined(DEBUG)
			mLibPath = "addon" + pName + "_d.dylib";
		#else
			mLibPath = "addon" + pName + ".dylib";
		#endif

		DBUG_LO("using OSX to load shared library " << pName);

		mLibHandle = dlopen(mLibPath.c_str(), RTLD_LAZY);
		if(mLibHandle != NULL){

			draftFoo fn = reinterpret_cast<draftFoo>(dlsym(mLibHandle, "draftAll"));

			if( fn != NULL ){
				(*fn)(gWorld.get().get());
			}else{
				DBUG_LO("Unable to use shared library function 'draftAll' because " << dlerror());
			}

		}else{
			DBUG_LO("unable to load shared library '" << mLibPath << "' because " << dlerror() );
		}
	}
}

void
cAddon_OSX::closeAddon(){
	DBUG_LO("closing addon " << mLibPath);
	if( mLibHandle != NULL ){
		void (*fn)();
		fn = reinterpret_cast<void(*)()>(dlsym(mLibHandle, "closeLib"));

		if( fn != NULL ){
			(*fn)();
		}else{
			DBUG_LO("unable to close shared library '" << mLibPath << "' because " << dlerror() );
		}
		dlclose(mLibHandle);
	}
}

cAddon_OSX::cAddon_OSX():
	mLibHandle(NULL)
{}

cAddon_OSX::~cAddon_OSX(){
	closeAddon();
}
