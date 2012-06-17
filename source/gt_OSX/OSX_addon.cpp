#include "OSX_addon.hpp"

using namespace gt;

void
cAddon_OSX::draftAddon(const dText &refAddonName){
	if( mLibHandle == NULL ){
		mLibPath = "addon" + refAddonName.t;
		#if defined(DEBUG) && defined(GT_THREADS)
			mLibPath += "_dt.dylib";
		#elif defined(DEBUG)
			mLibPath += "_d.dylib";
		#elif defined(GT_THREADS)
			mLibPath += "_t.dylib";
		#else
			mLibPath += ".dylib";
		#endif

		DBUG_LO("using OSX to load shared library " << mLibPath);

		mLibHandle = dlopen(mLibPath.c_str(), RTLD_LAZY);
		if(mLibHandle != NULL){

			draftFoo fn = reinterpret_cast<draftFoo>(dlsym(mLibHandle, "draftAll"));

			if( fn != NULL ){
				(*fn)(gWorld.get().get());
			}else{
				WARN_S("Unable to use shared library function 'draftAll' because " << dlerror());
			}

		}else{
			WARN_S("unable to load shared library '" << mLibPath << "' because " << dlerror() );
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
