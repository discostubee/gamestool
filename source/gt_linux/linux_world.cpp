#include "gt_base/context.hpp"
#include "linux_world.hpp"

#include <fstream>
#include <glib.h>
#include <gio/gio.h>
#include <glib/gstdio.h>

////////////////////////////////////////////////////////////
using namespace gt;

const dPlaChar * cLinuxWorld::ADDON_PREFIX = "addon";

const dPlaChar * cLinuxWorld::ADDON_POSTFIX =
#if defined(DEBUG) && defined(GT_THREADS)
	"_dt.so";
#elif defined(DEBUG)
	"_d.so";
#elif defined(GT_THREADS)
	"_t.so";
#else
	".so";
#endif


const dPlaChar * cLinuxWorld::ADDON_CACHE_FILE = "addoncache.txt";
const dPlaChar * cLinuxWorld::LOG_FILE = "log.txt";

cLinuxWorld::cLinuxWorld(){
	memset(&tempTime, 0, sizeof(tempTime));
}

cLinuxWorld::~cLinuxWorld(){
	try{
		closeWorld();
	}catch(std::exception &e){
		excep::delayExcep::add(e.what());
	}
}

dMillisec
cLinuxWorld::getAppTime(){
	return getLinuxTime();
}

void
cLinuxWorld::loop(){
	cContext worldContext;
	while(mKeepLooping){
		mRoot->run(&worldContext);
		flushLines();
	}
}

void
cLinuxWorld::flushLines(){
	dLines lines;
	primordial::gPrim.getLines(&lines, true);

	if(lines.empty())
		return;

	std::fstream fs(LOG_FILE, std::fstream::out | std::fstream::app);
	for(dLines::iterator l=lines.begin(); l != lines.end(); ++l)
		fs << *l << std::endl;
}

tAutoPtr<cWorld>
cLinuxWorld::makeWorld(){
	return tAutoPtr<cWorld>(new cLinuxWorld());
}

void
cLinuxWorld::openAddon(const dStr &name){
	dNameHash hash = makeHash(name.c_str());

	if(mLibHandles.find(hash) != mLibHandles.end()){
		DBUG_LO("addon " << name << " already open");
		return;
	}

	dStr libPath = dStr("./") + ADDON_PREFIX + name + ADDON_POSTFIX;

	DBUG_LO("using Linux to load shared library " << libPath);

	void *handLib = dlopen(libPath.c_str(), RTLD_NOW | RTLD_LOCAL);
	if(handLib != NULL){
		mLibHandles[hash] = handLib;

		draftFoo fn = reinterpret_cast<draftFoo>(dlsym(handLib, "draftAll"));

		mBluesFromAddon = hash;
		if( fn != NULL ){
			(*fn)(this);
		}else{
			WARN_S("Unable to use shared library function 'draftAll' because " << dlerror());
		}
		mBluesFromAddon = 0;

		mOpenAddons.insert(hash);
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

		if(dlclose(found->second) != 0)
			WARN_S("Unable to close library '" << name << "' because " << dlerror());

	}else{
		WARN_S("Addon " << name << " handle was null.");
	}

	mLibHandles.erase(found);

	dRefAddons::iterator foundOpen = mOpenAddons.find(hash);
	if(foundOpen != mOpenAddons.end())
		mOpenAddons.erase(foundOpen);
}

void
cLinuxWorld::getAddonList(dAddons &output){
	std::list<dStr> addons;
	dStr search = ADDON_PREFIX + dStr("*") + ADDON_POSTFIX;
	getDirContents("./", search, false, addons);
	output.clear();

	for(std::list<dStr>::iterator itr = addons.begin(); itr != addons.end(); ++itr){
		dStr name;

		getAddonNameFromFilename(itr->c_str(), &name);
		output.insert(dAddons::value_type(
			makeHash(toNStr(name)),
			name
		));
	}
}

void
cLinuxWorld::readAddonCache(const dAddons &addons, dBlue2Addons &outMap, dAddon2Fresh &outFresh){
	char buff[512];
	std::fstream f(ADDON_CACHE_FILE, std::ios::in);
	dBlue2Addons::iterator current;

	for(dAddons::const_iterator itr = addons.begin(); itr != addons.end(); ++itr)
		outFresh.insert(
			dAddon2Fresh::value_type(itr->first, false)
		);

	if(!f.is_open())
		return;

	while(f.getline(buff, 512).good()){

	}
}

void
cLinuxWorld::writeAddonCache(const dBlue2Addons &info){
	std::fstream f(ADDON_CACHE_FILE, std::ios::out | std::ios::trunc);

	if(!f.is_open()){
		WARN_S("Couldn't open " << ADDON_CACHE_FILE);
		return;
	}

	typedef std::map< dStr, std::set<dStr> > dAddon2Blues;
	dAddon2Blues rinfo;

	dAddons::iterator found;
	for(dBlue2Addons::const_iterator blue = info.begin(); blue != info.end(); ++blue){
		for(dRefAddons::const_iterator ads = blue->second.begin(); ads != blue->second.end(); ++ads){
			found = mAvailableAddons.find(*ads);
			if(found == mAvailableAddons.end())
				continue;

			rinfo[found->second].insert(blue->first);
		}
	}

	GStatBuf fileInfo;	//- Refer to posix <sys/stat.h>
	dStr filename;
	for(dAddon2Blues::iterator ads = rinfo.begin(); ads != rinfo.end(); ++ads){
		filename = dStr("./") + ADDON_PREFIX + ads->first + ADDON_POSTFIX;

		if(-1 == g_stat(
			static_cast<const gchar*>(filename.c_str()),
			&fileInfo
		)){
			WARN_S("Unable to get stats on file " << filename);
			continue;
		}

		f << ads->first << ":" << fileInfo.st_mtime << std::endl;
		for(std::set<dStr>::iterator blue = ads->second.begin(); blue != ads->second.end(); ++blue){
			f << "	" << *blue << std::endl;
		}
	}
}

void
cLinuxWorld::getAddonNameFromFilename(const dPlaChar *filename, dStr *output){
	ASRT_NOTNULL(filename);
	ASRT_NOTNULL(output);

	size_t len = strnlen(filename, MAX_SRTBUFF);
	char const* start = strrchr(filename, static_cast<int>('/'));

	start = strstr(start == NULL ? filename : start, ADDON_PREFIX);
	if(start == NULL || static_cast<size_t>(start - filename) + strlen(ADDON_PREFIX) > len)
		THROW_ERROR(filename << " isn't an addon name.");

	start += sizeof(char) * strlen(ADDON_PREFIX);
	char const* end = strstr(start, ADDON_POSTFIX);
	if(end == NULL){
		end = strstr(start, ".cpp");
	}

	if(end == NULL)
		THROW_ERROR(filename << " is missing the postfix.");

	output->assign(start, static_cast<size_t>(end - start) / sizeof(char));
}

dMillisec
cLinuxWorld::getLinuxTime(){
	gettimeofday(&tempTime, NULL);

	return tempTime.tv_usec;
}

void
cLinuxWorld::getDirContents(const dStr &dir, const dStr &search, bool dirsOnly, std::list<dStr> &output){
	GError *errors=NULL;
	gchar const **encodingList=NULL; //- Do NOT free
	gchar const *nativeEncoding=NULL;	//- Do NOT free
	gsize charsConverted=0;
	gsize lengthNative=0;
	GDir *handDir=NULL;	//- use glib to free
	gchar *platformFName=NULL;	//- free me
	size_t lenPlatformFName=0;

	g_get_filename_charsets(&encodingList);

	if(encodingList==NULL || encodingList[0]==NULL)
		THROW_ERROR("Encoding list no good.");

	g_get_charset(&nativeEncoding);

	if(nativeEncoding==NULL)
		THROW_ERROR("Couldn't get native encoding.");

	platformFName = g_convert(
		(const gchar*)dir.c_str(), (const gssize)dir.size(),
		encodingList[0], nativeEncoding,
		&charsConverted, &lengthNative, &errors
	);

	if(platformFName==NULL)
		THROW_ERROR("Platform name is null.");

	handDir = g_dir_open(platformFName, 0, &errors);

	char *nativeFName;
	gchar const *refName;	//- Do NOT free
	gchar *fullPath;
	do{
		refName = g_dir_read_name(handDir);

		if(refName==NULL)
			break;

		nativeFName=NULL;

		fullPath = g_build_filename(platformFName, refName, NULL);

		if(g_file_test(fullPath, G_FILE_TEST_IS_DIR) != dirsOnly){
			g_free(fullPath);
			continue;
		}

		lenPlatformFName = strlen(refName);

		nativeFName = (char*)g_convert(	//- nativeFName should be freed by the output file list
			refName, lenPlatformFName,
			nativeEncoding, encodingList[0],
			&charsConverted, &lengthNative, &errors
		);

		if(hasWildStr(nativeFName, search.c_str())){
			output.push_back(nativeFName);
			continue;
		}

	}while(refName!=NULL);

	g_dir_close(handDir);
}

dStr
gt::getPlatformInfo(){
	dStr out("Linux");
#ifdef GT_THREADS
	out += ", Threaded";
#endif
	return out;
}

