#include "OSX_world.hpp"
#include <CoreFoundation/CoreFoundation.h>
#include <CoreServices/CoreServices.h>
#include <fstream>

void OSXError(CFErrorRef ref){
	if(ref == NULL)
		return;

	CFStringRef msg = CFErrorCopyDescription(ref);
	char buff[512];

	if(CFStringGetCString(
		msg,
		buff,
		512,
		kCFStringEncodingASCII
	))
		WARN_S(buff);

	CFRelease(msg);
}

////////////////////////////////////////////////////////////
using namespace gt;

const char * cOSXWorld::ADDON_POSTFIX =
#if defined(DEBUG) && defined(GT_THREADS)
	"_dt.dylib";
#elif defined(DEBUG)
	"_d.dylib";
#elif defined(GT_THREADS)
	"_t.dylib";
#else
	".dylib";
#endif

const char * cOSXWorld::ADDON_CACHE_FILE = "addoncache.txt";

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
		try{
			closeWorld();
		}catch(std::exception &e){
			WARN(e);
		}
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
		flushLines();
	}
}

void
cOSXWorld::flushLines(){
	mLines->clear();
	excep::delayExcep::shake();
}

tAutoPtr<cWorld>
cOSXWorld::makeWorld(){
	return tAutoPtr<cWorld>(new cOSXWorld());
}

void
cOSXWorld::openAddon(const dStr &name){
	dNameHash hash = makeHash(name.c_str());

	if(mLibHandles.find(hash) != mLibHandles.end()){
		DBUG_LO("addon " << name << " already open");
		return;
	}

	dAddons::iterator found = mAvailableAddons.find(hash);
	if(found == mAvailableAddons.end()){
		WARN_S("Addon " << name << " not approved.");
		return;
	}

	dStr libPath = "addon" + name + ADDON_POSTFIX;

	DBUG_LO("using OSX to load shared library " << name << ", with path " << libPath);

	void *handLib = dlopen(libPath.c_str(), RTLD_LAZY);

	if(handLib == NULL){
		WARN_S("unable to load shared library '" << libPath << "' because " << dlerror() );
		return;
	}

	mLibHandles[hash] = handLib;

	draftFoo fn = reinterpret_cast<draftFoo>(dlsym(handLib, "draftAll"));

	if( fn != NULL ){
		(*fn)(this);
		mOpenAddons.insert(hash);
	}else{
		WARN_S("Unable to use shared library function 'draftAll' because " << dlerror());
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
	mOpenAddons.erase( mOpenAddons.find(hash) );
}

void
cOSXWorld::getAddonList(dAddons &output){
	char buff[256];
	dStr search = dStr("addon*") + ADDON_POSTFIX;

	static const size_t prefixLen = strlen("addon");
	static const size_t postfixLen = strlen(ADDON_POSTFIX);

	SInt32 error;

	snprintf(buff, 256, "./");

	CFURLRef dirURL = CFURLCreateFromFileSystemRepresentation(
		NULL,
		reinterpret_cast<unsigned char*>(buff),
		strlen(buff),
		true
	);

	CFArrayRef files = static_cast<CFArrayRef>(CFURLCreatePropertyFromResource(
		NULL, dirURL, kCFURLFileDirectoryContents, &error
	));

	try{
		CFIndex numFiles = CFArrayGetCount(files);
		while(numFiles > 0){
			--numFiles;

			CFURLRef f = static_cast<CFURLRef>(CFArrayGetValueAtIndex(
				files, numFiles
			));

			if(CFURLHasDirectoryPath(f))
				continue;

			if(!CFURLGetFileSystemRepresentation(
				f,
				false,
				reinterpret_cast<unsigned char*>(buff),
				256
			))
				continue;

			if(hasWildStr(buff, search.c_str())){
				size_t len = strlen(buff);
				if(len > prefixLen + postfixLen){
					buff[len - postfixLen] = '\0';
					output.insert( dAddons::value_type(
						makeHash(&buff[prefixLen]),
						&buff[prefixLen]
					));
				}
			}
		}
	}catch(...){
	}

	CFRelease(files);
	CFRelease(dirURL);
}

void
cOSXWorld::readAddonCache(const dAddons &addons, dBlue2Addons &outMap, dAddon2Fresh &outFresh){
	char buff[512];
	std::fstream f(ADDON_CACHE_FILE, std::ios::in);
	dBlue2Addons::iterator current;

	CFTimeZoneRef timezone = CFTimeZoneCopyDefault();

	for(dAddons::const_iterator itr = addons.begin(); itr != addons.end(); ++itr)
		outFresh.insert(
			dAddon2Fresh::value_type(itr->first, false)
		);

	if(!f.is_open())
		return;

	while(f.getline(buff, 512).good()){
		if(buff[0] == '\t' && current != outMap.end()){

			char * year = strchr(&buff[1], static_cast<int>(':'));
			if(year == NULL)
				continue;
			year[0] = '\0'; ++year;

			char * month = strchr(year, static_cast<int>(','));
			if(month == NULL)
				continue;
			month[0] = '\0'; ++month;

			char * day = strchr(month, static_cast<int>(','));
			if(day == NULL)
				continue;
			day[0] = '\0'; ++day;

			char * hour = strchr(day, static_cast<int>(','));
			if(hour == NULL)
				continue;
			hour[0] = '\0'; ++hour;


			dStr addonFile = "./addon";
			addonFile += &buff[1];
			addonFile += ADDON_POSTFIX;

			CFErrorRef err = NULL;
			CFDateRef date = NULL;
			CFURLRef addonURL = CFURLCreateFromFileSystemRepresentation(
				NULL,
				reinterpret_cast<const unsigned char *>(addonFile.c_str()),
				addonFile.length(),
				true
			);

			try{
				if(CFURLCopyResourcePropertyForKey(
					addonURL,
					kCFURLCreationDateKey,
					static_cast<void*>(&date),
					&err
				)){

					CFGregorianDate gdate = CFAbsoluteTimeGetGregorianDate(
						CFDateGetAbsoluteTime(date),
						timezone
					);

					dNameHash hash = makeHash(&buff[1]);
					outFresh[hash]	= gdate.year == atoi(year)
									&& gdate.month == atoi(month)
									&& gdate.day == atoi(day)
									&& gdate.hour == atoi(hour)
					;

					current->second.insert(hash);
				}
			}catch(...){
			}

			if(date != NULL)
				CFRelease(date);

			if(addonURL != NULL)
				CFRelease(addonURL);

			OSXError(err);
		}else{
			current = outMap.insert(
				outMap.begin(),
				dBlue2Addons::value_type(buff, dRefAddons())
			);
		}
	}

	CFRelease(timezone);
}

void
cOSXWorld::writeAddonCache(const dBlue2Addons &info){
	std::fstream f(ADDON_CACHE_FILE, std::ios::out | std::ios::trunc);

	if(!f.is_open()){
		WARN_S("Couldn't open " << ADDON_CACHE_FILE);
		return;
	}

	CFTimeZoneRef timezone = CFTimeZoneCopyDefault();

	dRefAddons::iterator addon;
	dAddons::iterator addonName;
	for(dBlue2Addons::const_iterator blue = info.begin(); blue != info.end(); ++blue){

		f << blue->first << std::endl;
		for(addon = blue->second.begin(); addon != blue->second.end(); ++addon){
			addonName = mAvailableAddons.find(*addon);
			dStr addonFile = "./addon" + addonName->second + ADDON_POSTFIX;
			CFErrorRef err = NULL;

			CFURLRef addonURL = CFURLCreateFromFileSystemRepresentation(
				NULL,
				reinterpret_cast<const unsigned char *>(addonFile.c_str()),
				addonFile.length(),
				true
			);

			CFDateRef date = NULL;

			try{
				if(CFURLCopyResourcePropertyForKey(
					addonURL,
					kCFURLCreationDateKey,
					static_cast<void *>(&date),
					&err
				)){
					CFGregorianDate gdate = CFAbsoluteTimeGetGregorianDate(
						CFDateGetAbsoluteTime(date),
						timezone
					);

					f << "	" << addonName->second
					  << ":" << static_cast<int>(gdate.year)
					  << "," << static_cast<int>(gdate.month)
					  << "," << static_cast<int>(gdate.day)
					  << "," << static_cast<int>(gdate.hour)
					  << std::endl;
				}

			}catch(...){
			}

			if(date != NULL)
				CFRelease(date);

			if(addonURL != NULL)
				CFRelease(addonURL);

			OSXError(err);
		}
	}

	CFRelease(timezone);
}

std::vector<dStr>
cOSXWorld::getFiles(const dStr &search){

	std::vector<dStr> rtn;

	CFURLRef dirURL = CFURLCreateFromFileSystemRepresentation(
		NULL,
		(const unsigned char *)search.c_str(),
		search.size(),
		true
	);

	try{
		SInt32 error;

		CFArrayRef files = static_cast<CFArrayRef>(CFURLCreatePropertyFromResource(
			NULL,
			dirURL,
			kCFURLFileDirectoryContents,
			&error
		));

		if(error != 0 || files == NULL)
			THROW_ERROR("can't open directory");

		char buff[256];

		CFIndex numFiles = CFArrayGetCount(files);
		while(numFiles > 0){
			--numFiles;

			//- There should be no need to free or retain this (no multithread)
			CFURLRef tmp = (CFURLRef)CFArrayGetValueAtIndex(files, numFiles);
			if(true == CFURLHasDirectoryPath(tmp))
				continue;

			if(false == CFURLGetFileSystemRepresentation(tmp, false, (unsigned char*)buff, 256))
				continue;
		}

		CFRelease(files);
	}catch(...){
	}

	CFRelease(dirURL);

	return rtn;
}
