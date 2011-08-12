#include "linux_addon.hpp"

using namespace gt;

void
cAddon_linux::draftAddon(const std::string &pName){
	if( mLibHandle == NULL ){
		mLibPath = "./" + pName + ".so";

		DBUG_LO("using linux to load shared library " << pName);

		mLibHandle = dlopen(mLibPath.c_str(), RTLD_LAZY);
		if(mLibHandle != NULL){
			void (*fn)(cWorld*);

			fn = reinterpret_cast<void(*)(cWorld*)>(dlsym(mLibHandle, "draftAll"));

			if( fn != NULL ){
				(*fn)(gWorld);
			}else{
				DBUG_LO("Unable to use shared library function 'draftAll' because " << dlerror());
			}

		}else{
			DBUG_LO("unable to load shared library '" << mLibPath << "' because " << dlerror() );
		}
	}
}

void
cAddon_linux::closeAddon(){
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

cAddon_linux::cAddon_linux():
	mLibHandle(NULL)
{
}

cAddon_linux::~cAddon_linux(){
	closeAddon();
}
