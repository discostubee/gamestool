/*
 *!\file	world.hpp
 *!\brief	This is the starting point for the source code dependency (ignoring some included utility source).
 *
 **********************************************************************************************************
 *  Copyright (C) 2010  Stuart Bridgens
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License (version 3) as published by
 *  the Free Software Foundation.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *********************************************************************************************************
 *
 * Seeing as how it's our sort of, starting point: We'll add all the kind of info that's useful right here.
 *
 *
 *!\note	A little bit about the short hand being used in this project.
 *! cSomething		An complex object name, either a class or struct which has methods.
 *! iSomething		Interface, can contain some implementations but is mostly pure virtual.
 *! sSomething		A simple object or data container. In other words, a struct with no defined methods.
 *! eSomething		an enum, for both the scope and the values.
 *! mSomething		Variable data stored in a class or structure, otherwise known as a member variable.
 *! pSomething		A parameter passed into a function call.
 *! uSomething		Constant and constant static data where the u stands for unchanging. Not to replace p when constant data is passed to a function.
 *! tSomething		A template class or struct.
 *! xSomething		Static data as either a class member or a function variable.
 *! dSomething		A type definition.
 *! SOMETHING		A pre-processor macro or value.
 *! gSomething		A global variable.
 *! ptrSomething	A managed pointer.
 *! fPtrSomething	A function pointer.
 *! patSomething	A patch through function used with commands.
 *! roSomething		A route to something. This is for pointers that you should not cleanup  because it's expected that the route exists longer than what is using it.
 *! mroSomething	A route that's contained as a member. Don't clean these up, which also means making sure it lasts longer than the containing class.
 *! STAMP_SOMETHING	This is when a macro expands into a declaration/definition. In other words, it stamps/prints out something.
 *
 *!\note	A little bit about some of the terms used around the place. This isn't shorthand exactly, so it get's it's own note coz it's special.
 *! set		Set a primitive to a value, copy a string, reference a static constant object, deep copy an object.
 *! copy	Copy a stream/buffer
 *! get		Return a primitive by copy, return a stream/buffer copy, return a reference to an object
 *! pass	Passes back something as a reference.
 *! link	Set a smart pointer to different, already existing, reference.
 *! make	Return a fresh new instance of an object from a factory.
 *! clone	Make a duplicate of an object instance and return a smart pointer to the new clone.
 *! clear	Empty something so that it doesn't contain any data.
 *! blank	Change a link to being a blank or dead end (terminator) object.
 *! take	The function will manage (cleanup) the memory it is being passed. In other word the object or function becomes the custodian of this memory.
 *! eat		A function that consumes a buffer parameter.
 *
 *!\note	Some notes about the terminology for the jack interface
 *!	When manipulating a figment, we use the jack function, which reads a command, which contains plugs, which are wrappers to data. In a sense
 *!	jacking can easily be thought of as the same as using old style AV cables (using the term lead because it's 2 characters less), the ones
 *!	with red/white and yellow plugs. We say that not because every lead has only 3 plugs, but because those cables were single leads with
 *!	multiple independent plugs on the end.
 *!
 *!	When the jack interface is used, it uses the command's function pointer to call a method within the figment. The functions pointed to by the
 *!	command are known as a patcher (as in, patching through with a lead), and abbreviated to 'pat' IE patSomething. A patch through function can
 *!	handle the command all by itself, or it can pass it onto another function and take advantage of standard polymorphism.
 */

#ifndef	WORLD_HPP
#define WORLD_HPP

///////////////////////////////////////////////////////////////////////////////////
// Preprocessor config
#ifdef DEBUG
	//!\brief	Defined so that all debug versions are verbose at the moment.
#	define DBUG_VERBOSE
#endif

///////////////////////////////////////////////////////////////////////////////////
// Includes
#include "iFigment.hpp"
#include "threadTools.hpp"
#include "profiler.hpp"

#include <set>
#include <stdarg.h>

////////////////////////////////////////////////////////////////////
// Macros

#ifdef WIN32
	//#define DYN_LIB_IMP_DEC(rnt) extern "C" __declspec(dllimport) rnt __stdcall
#	define DYN_LIB_EXP_DEC(rnt) extern "C" __declspec(dllexport) rnt
#	define DYN_LIB_DEF(rnt) __declspec(dllexport) rnt
#else
	//#define DYN_LIB_IMP_DEC(rnt) extern "C" rnt __stdcall
#	define DYN_LIB_EXP_DEC(rnt) extern "C" rnt
#	define DYN_LIB_DEF(rnt) rnt
#endif

#define WARN(x)\
	gt::cWorld::primordial::warnError(x, __FILE__, __LINE__)

#define WARN_S(x)\
	{ std::stringstream ss; ss << x; gt::cWorld::primordial::warnError(ss.str().c_str(), __FILE__, __LINE__); }

// Handy for all those (...) catch blocks.
extern const char *MSG_UNKNOWN_ERROR;
#define UNKNOWN_ERROR	WARN_S(MSG_UNKNOWN_ERROR);

#ifdef GTUT
	//- Adds line flushing per test. Sadly, if a test fails the lines are not flushed until the next test. At this point I can't see a way to fix this.
#	undef GTUT_END
#	define GTUT_END\
		catch(excep::base_error &e){ GTUT_ASRT(false, e.what()); }  gt::gWorld.get()->flushLines(); }
#endif

#ifdef DEBUG
#	define PROFILE\
		cProfiler::cToken profileToken = gt::cWorld::primordial::makeProfileToken(__FILE__, __LINE__)
#	define DBUG_LO(x)\
		{ std::stringstream ss; ss << x; gt::cWorld::primordial::lo(ss.str()); }
#else
#	define PROFILE
#	define DBUG_LO(x)
#endif

#if defined(DBUG_VERBOSE) && defined(DEBUG)
#	define DBUG_VERBOSE_LO(x)\
		{ std::stringstream ss; ss << x; gt::cWorld::primordial::lo(ss.str()); }
#else
#	define DBUG_VERBOSE_LO(x)
#endif


////////////////////////////////////////////////////////////////////
// Typedefs
namespace gt{
	typedef dIDSLookup dConSig;		//!< This is the signature of a context.
}


////////////////////////////////////////////////////////////////////
// Classes
namespace gt{

	//---------------------------------------------------------------------------------------------------
	//!\brief	The world is both a factory and a service provider for pretty much every figment.
	//!\note	Not threadsafe, so it must be accessed with a tMrSafety.
	//!\todo	Prevent a collection of objects becoming an island which is separate from the root node, and
	//!			thus will never be cleaned up. This will also be a huge problem when removing addons where
	//!			the objects made in the addon need to be blanked.
	class cWorld{
	public:

		//--------------------------------------------------------
		// New types.
		typedef std::list<dStr> dLines;
		typedef tShortLookup<cContext*> dContextLookup;

		//--------------------------------------------------------
		// Members
		bool mKeepLooping;

		//--------------------------------------------------------
		// Regular world methods
		cWorld();
		virtual ~cWorld();

		void setRoot(ptrFig pNewRoot);

		//--------------------------------------------------------
		// Blueprint stuff
		void addBlueprint(cBlueprint* pAddMe);	//!<	Adds a blueprint to the library. Will replace a blueprint if the new ones say to.
		const cBlueprint* getBlueprint(dNameHash pNameHash);

		//!\brief	Removed, or un-draft, a blueprint from the world.
		//!\note	Super slow.
		//!\todo	Figure out a way to avoid traversing the program tree every time.
		//!\todo	make pRemoveMe a pointer to a pointer, so that it can be turned into a NULL.
		void removeBlueprint(const cBlueprint* pRemoveMe);

		//--------------------------------------------------------
		// Factory outlets
		ptrFig makeFig(dNameHash pNameHash);	//!< Makes a new figment that is managed by a smart pointer.
		ptrFig makeFig(const dPlaChar *pName);	//!< Handy function if you want to use literal strings in a demo.
		ptrLead makeLead(unsigned int pComID);	//!< Makes a new lead that is managed by a smart pointer.

		//!\brief	If you don't have the context ID (possible because you're creating some kind of hard coded demo), you can still
		//!			get a lead if you have the string name of the figment and the context it came from.
		ptrLead makeLead(const dPlaChar *aFigName, const dPlaChar *aComName);

		//--------------------------------------------------------
		// Register office

		//!\brief	We need to keep track of the number of contexts so we can make a context lookup.
		//!\note	Locks all contexts while the lookup table is changed.
		dConSig regContext(cContext* pCon);

		//!\brief	NEVER forget to do this. Locks all contexts while the table is updated. Previous context signatures
		//!			(apart from the one being unregged) remain valid.
		void unregContext(dConSig pSig);

		bool activeContext(dConSig pSig);	//!< Is this context still alive?

		void lazyCloseAddon(const dPlaChar* pAddonName);	//!< This is called from addon dependent figments, so we can't directly call closeAddon.

		//--------------------------------------------------------
		// Get stuff
		
		//!\brief	Use this if you know the hash of the figment and the ID of the plug tag.
		//!\note	Throws if not found.
		const cPlugTag* getPlugTag(dNameHash pFigHash, unsigned int pPTHash);

		//!\note	Useful when writing demos where you are using addons and you don't want to include the headers
		const cPlugTag* getPlugTag(const dPlaChar *figName, const dPlaChar *tagName);

		//!\breif	Tries to find a plug tag in all the current blueprints it has.
		//!\note	Throws if not found.
		//!\todo	Make this more efficient by preventing a rescan of every blueprint.
		const cPlugTag* getPlugTag(unsigned int aID);

		//!\brief	Instead of making a new empty figment every time, we might as well share the same village
		//!			bicycle.
		ptrFig getEmptyFig();

		//!\brief	Intended to be used by plugs so they can have quick access to the context lookup.
		//!\note	This lookup table should not change unless all the contexts have first been locked.
		const dContextLookup& getContextLookup();

		//--------------------------------------------------------
		// Polymorphs

		virtual dMillisec getAppTime() =0;
		virtual void loop() =0;	//!< Enter the main program loop. Continues looping until it is told to stop.
		virtual void flushLines	() =0;		//!< Process the lines to be displayed on the console.
		virtual void openAddon(const dStr &name) =0;		//!< Opens an addon with the given name

		//---------------------------------------------------------------------------------------------------
		//!\brief	primodial is used to manage services that are present before a world is available.
		//!\note	Not a namespace so that data is hidden.
		class primordial{
		public:

			//!\brief	Add a line to be displayed in the console.
			//!\note	Threadsafe: Has a specific mutex lock to acquire and release.
			static void lo(const dStr& pLine, bool cleanup=false);

			//!\brief	logs a warning. Note that fatal errors are caught by the catch at the top of the program; there's no need for a world function to handle it.
			static void warnError(const char *msg, const char* pFile, const unsigned int pLine);

			//!\brief	Allows you to pass the error type directly.
			static void warnError(excep::base_error &pE, const char* pFile, const unsigned int pLine);

#			ifdef GTUT
				static void suppressNextError();	//!< Helpful when running tests where we expect at most 1 error. This isn't to be used outside of testing.
#			endif

			//!\brief	Makes a profile token using the profiler stored in this world.
			//!\note	Using the world to manage the profiler so data can be copied from the worlds inside addons.
			//!\todo	Make the death report threadsafe
			static cProfiler::cToken makeProfileToken(const char* pFile, unsigned int pLine, bool cleanup=false);

			//!\todo	Make threadsafe.
			static void makeProfileReport(std::ostream &log);

			static void redirectWorld(gt::cWorld *directHere);	//!< copies the primordial static data into the given world, and redirects gWorld to point to the one given. If null, gWorld is cleaned up and set to null.

		protected:
#			ifdef GTUT
				static bool mSuppressError;	//!<
#			endif

#			ifdef GT_THREADS
				static boost::recursive_mutex *xProfileGuard;
				static boost::recursive_mutex *xLineGuard;
#			endif

			//--------------------------------------------------------
			// Data which must be redirected if this is an addon's heap.
			// This stuff also needs specific setup
			static gt::cWorld::dLines* xLines;
			static cProfiler* xProfiler;

			static void cleanup();

			//--------------------------------------------------------
			virtual void closeAddon(const dStr &name) =0;	//!< Actually close the addon

			friend class gt::cWorld;

		private:
			primordial();
		};


	protected:

		// references from primordial.
		dLines* mLines;	//!< points to the static global that holds all the lines that need flushing.
		cProfiler* mProfiler;	//!< points to the static global.
#		ifdef GT_THREADS
			boost::recursive_mutex *mProfileGuard;
			boost::recursive_mutex *mLineGuard;
#		endif

		ptrFig mRoot;
		std::list<dStr> mAddonsToClose;	//!< List of addons to close next cycle.

		friend class primordial;

	private:
		struct sBlueprintHeader;

		typedef std::map<dNameHash, sBlueprintHeader> dBlueprintMap;

		static bool thereCanBeOnlyOne;	//!< You can only create and destroy the world once (in the same heap).

		dBlueprintMap mBlueprints; //!< Blueprint library is static because we only ever want 1 blueprint library.
		dBlueprintMap mBlueArchive; //!< Archives a blueprint here when it is replaced.
		std::vector<const cCommand*> mCommands;
		dBlueprintMap::iterator mScrBMapItr;	//!< scratch variable for iterating over blueprint library.
		ptrFig mVillageBicycle;	//!< Used for empty figment.
		bool mBicycleSetup;	//!< Faster than looking for it in the library every time.
		dContextLookup mContexts; //!< We keep track of all the contexts here in the world.

		cWorld& operator=(cWorld&) { return *this; };	//!< Banned.
	};

	extern tMrSafety<cWorld> gWorld;	//!< Gives you threadsafe access to the world.


	//---------------------------------------------------------------------------------------------------
	//!\brief	Any figments that come from an addon are dependent on that addon (so you inherit from it).
	//!			When no more of these figments are open, the world closes the addon.
	//!\note	Expects ADDON to be a class with member "static const dPlaChar* getAddonName()"
	template<typename ADDON>
	class tAddonDependent{
	private:
		static int instCount;

	public:
		tAddonDependent(){
			++instCount;
		}

		virtual ~tAddonDependent(){
			try{
				--instCount;
				if(instCount<=0)
					gWorld.get()->lazyCloseAddon( ADDON::getAddonName() );
			}catch(...){}
		}

   		virtual const dPlaChar* requiredAddon() const { return ADDON::getAddonName(); }	//!< Returns name of addon that this figment comes from. An empty string means that no addon is required.
	};

	template<typename ADDON> int tAddonDependent<ADDON>::instCount = 0;
}


///////////////////////////////////////////////////////////////////////////////////
// Typedefs
namespace gt{
	//typedef tPtrRef<iFigment> refFig;	//!< Used when you want access to a figment
}


#endif

