/*
 *!\file	world.hpp
 *!\brief	This is the starting point for the source code dependency (ignoring some included utility source).
 *!\todo	Make world access thread safe.
 *
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
 *
 *!\note	This file, seeing as how it's our sort of, starting point: We'll add all the kind of info that's useful right here.
 *!\note	A little bit about the short hand being used in this project.
 *! oSomething		An object name, either a class or struct.
 *! eSomething		an enum, for both the scope and the values.
 *! mSomething		Variable data stored in a class or structure, otherwise known as a member variable.
 *! aSomething		An argument passed into a function call.
 *! uSomething		Constant and constant static data where the u stands for unchanging. Not to replace p when constant data is passed to a function.
 *! tSomething		A template class or struct.
 *! xSomething		Static data as either a class member or a function variable.
 *! dSomething		A type definition.
 *! SOMETHING		A pre-processor macro or value.
 *! gSomething		A global variable.
 *! ptrSomething	A managed pointer.
 *! fPtrSomething	A function pointer.
 *
 *\note		A little bit about some of the terms used around the place. This isn't shorthand exactly, so it get's it's own note coz it's special.
 *! set		Set a primitive to a value, copy a string, reference a static constant object, deep copy an object.
 *! copy	Copy a stream/buffer
 *! get		Return a primitive by copy, return a stream/buffer copy, return a reference to an object
 *! link	Set a smart pointer to different, already existing, reference.
 *! make	Return a fresh new instance of an object from a factory.
 *! clone	Make a duplicate of an object instance and return a smart pointer to the new clone.
 *! clear	Empty something so that it doesn't contain any data.
 *! blank	Change a link to being a blank or dead end (terminator) object.
 *! take	The function will clean up the memory it is being passed. The object becomes the custodian of this memory.
 *!
 */

#ifndef	WORLD_HPP
#define WORLD_HPP

#include "threadTools.hpp"
#include "ptrTools.hpp"
#include "dirPtr.hpp"
#include "gt_string.hpp"
#include "utils.hpp"
#include "profiler.hpp"
#include "byteBuffer.hpp"

#include <list>
#include <set>
#include <stdarg.h>
#include <boost/shared_ptr.hpp>

///////////////////////////////////////////////////////////////////////////////////
// forward declarations
namespace gt{
	class cBase_plug;
	class cPlugTag;
	class cLead;
	class cBlueprint;
	class cCommand;
	class cContext;
	class iFigment;
}

////////////////////////////////////////////////////////////////////
// Typedefs
namespace gt{
	typedef const cContext* dConSig;	//!< This is the signature of a context.
	typedef tDirPtr<iFigment> ptrFig;	//!< Smart pointer to a figment.
	typedef boost::shared_ptr<cLead> ptrLead;	//!< Smart pointer to a lead.
	typedef const void* dFigSaveSig;	//!< This is used to uniquely identify a figment at save and load time.
}

///////////////////////////////////////////////////////////////////////////////////
// figment interface along with with supporting classes
namespace gt{

	//-------------------------------------------------------------------------------------
	//!\brief	helpful when loading.
	class cReload{
	public:
		ptrFig		fig;
		cByteBuffer	data;	//!< Accompanying reload data. Applied after all figments have been remade.

		cReload();

		//!\brief	Stores a figment before it has been passed the rest of its reloaded data. This is so that figment plugs
		//!			can see all the reloaded figments when they are passed the buffer.
		//!\param	pFig	Reference to the newly made figment (it's not reloaded until it has been passed the buffer.
		//!\param	copyMe	This is the accompanying reload buffer which has yet to be applied to the figment. The data
		//!					being pointed to is copied so that it exists at the right time.
		cReload(ptrFig pFig, const dByte* copyMe = NULL, size_t buffSize = 0);

		//!\brief	Cleans up the data it has copied.
		~cReload();
	};

	typedef std::map<dFigSaveSig, cReload*> dReloadMap;

	//-------------------------------------------------------------------------------------
	//!\brief	Figment interface, put here so we have a complete interface for the ptrFig type. Refer to the base implementation of this
	//!			class to get the low down on what all these methods mean.
	class iFigment{
	public:
		virtual ~iFigment() {}
		virtual const dNatChar* name() const =0;
		virtual dNameHash hash() const =0;

		virtual dNameHash getReplacement() const =0;
		virtual void jack(ptrLead pLead, cContext* pCon)=0;
		virtual void run(cContext* pCon)=0;
		virtual void save(cByteBuffer* pAddHere)=0;
		virtual void loadEat(cByteBuffer* pBuff, dReloadMap* pReloads = NULL)=0;
		virtual void getLinks(std::list<ptrFig>* pOutLinks)=0;
	};
}

////////////////////////////////////////////////////////////////////
// Classes
namespace gt{

	//---------------------------------------------------------------------------------------------------
	//!\class	cWorld
	//!\brief	The world is a single object that ties the program together, as well as being the main 
	//!			factory that creates figment-type objects. The world object is also a singleton that is 
	//!			seen by every figment-type object and offers services to them. It also designed to 
	//!			coordinate different heaps located in addons. Must also be threadsafe when accessed by
	//!			a mr safety.
	//!\todo	Prevent a collection of objects become an island which is separate from the root node, and
	//!			thus will never be cleaned up. This will also be a huge problem when removing addons where
	//!			the objects made in the addon need to be blanked.
	class cWorld{
	public:

		//--------------------------------------------------------
		// New types.
		typedef std::list<dStr> dLines;

		//--------------------------------------------------------
		// Members
		bool mKeepLooping;

		//--------------------------------------------------------
		// Logging and info services

		//!\brief	Add a line to be displayed in the console.
		//!\note	Threadsafe: Has a specific mutex lock to acquire and release.
		static void lo(const dStr& pLine);

		//!\brief	logs a warning. Note that fatal errors are caught by the catch at the top of the program; there's no need for a world function to handle it.
		void warnError(excep::base_error &pE, const dNatChar* pFile, const unsigned int pLine);

		//!\todo	Make threadsafe.
		static void makeProfileReport(std::ostream &log);

		//--------------------------------------------------------
		// Regular world methods
		cWorld();
		virtual ~cWorld();

		void setRoot(ptrFig pNewRoot);

		virtual void copyWorld(cWorld* pWorld);

		//--------------------------------------------------------
		// Blueprint stuff

		//!\brief	Adds a blueprint to the library. Will replace a blueprint if the new ones say to.
		void addBlueprint(const cBlueprint* pAddMe);

		const cBlueprint* getBlueprint(dNameHash pNameHash);

		//!\brief	Removed, or un-draft, a blueprint from the world.
		//!\note	Super slow.
		//!\todo	Figure out a way to avoid traversing the program tree every time.
		void removeBlueprint(const cBlueprint* pRemoveMe);

		//--------------------------------------------------------
		// Factory outlets

		//!\brief	Makes a new figment that is managed by a smart pointer.
		ptrFig makeFig(dNameHash pNameHash);

		//!\brief	Makes a new lead that is managed by a smart pointer.
		//!\param	pFigNameHash
		//!\param	pCommandID
		ptrLead makeLead(dNameHash pFigHash, dNameHash pComHash, dConSig pConx);

		//!\todo	Make threadsafe.
		static cProfiler::cToken makeProfileToken(const dNatChar* pFile, unsigned int pLine);

		//--------------------------------------------------------
		// Get references
		
		//!\note	Slow. Intended for ease of reading.
		const cPlugTag* getPlugTag(dNameHash pFigHash, dNameHash pPTHash);

		//!\brief	Instead of making a new empty figment every time, we might as well share the same village
		//!			bicycle.
		ptrFig getEmptyFig();

		//--------------------------------------------------------
		// Polymorphs
		virtual dMillisec	getAppTime	(){ return 0; }
		virtual void		loop		(){}	//!< Enter the main program loop. Continues looping until it is told to stop.
		virtual void		flushLines	();		//!< Process the lines to be displayed on the console. Uses cout by default

	protected:

		//--------------------------------------------------------
		// Data which must be redirected if this is an addon's heap.
		// This stuff also needs specific setup
		static dLines* xLines;
		static cProfiler* xProfiler;

		// Allows you to copy the statics from different heaps
		dLines* mLines;
		cProfiler* mProfiles;

		ptrFig mRoot;

		friend void redirectWorld(cWorld*);
		//--------------------------------------------------------

	private:
		struct sBlueprintHeader;

		typedef std::map<dNameHash, sBlueprintHeader> dBlueprintMap;

		dBlueprintMap mBlueprints; //!< Blueprint library is static because we only every want 1 blueprint library.
		dBlueprintMap mBlueArchive; //!< Archives a blueprint here when it is replaced.
		std::vector<const cCommand*> mCommands;
		dBlueprintMap::iterator mScrBMapItr;	//!< scratch variable for iterating over blueprint library.
		ptrFig mVillageBicycle;	//!< Used for empty figment.
		bool mBicycleSetup;	//!< Faster than looking for it in the library every time.

		static bool thereCanBeOnlyOne;	//!< You can only create and destroy the world once (in the same heap).

	};

	extern tMrSafety<cWorld> gWorld;	//!< Gives you threadsafe access to the world.
}

///////////////////////////////////////////////////////////////////////////////////
// Typedefs
namespace gt{
	typedef tPtrRef<iFigment> refFig;	//!< Used when you want access to a figment
}

////////////////////////////////////////////////////////////////////
// Functions
namespace gt{
	void redirectWorld(cWorld* pWorldNew);
}

////////////////////////////////////////////////////////////////////
// Macros
#ifdef DEBUG
	#define PROFILE	//cProfiler::cToken profileToken = gt::gWorld.get()->makeProfileToken(__FILE__, __LINE__)
	#define DBUG_LO(x) std::cout << x << std::endl; //{ std::stringstream ss; ss << x; gt::cWorld::lo(ss.str()); }
	
#else
	#define PROFILE
	#define DBUG_LO(x)
#endif

#if defined(DBUG_VERBOSE) && defined(DEBUG)
	#define DBUG_VERBOSE_LO(x) DBUG_LO(x)
#else
	#define DBUG_VERBOSE_LO(x)
#endif

#define WARN(x)	gt::gWorld.get()->warnError(x, __FILE__, __LINE__)

// Handy for all those (...) catch blocks.
#define UNKNOWN_ERROR	{ excep::unknownError temp(__FILE__, __LINE__); WARN(temp); }

#ifdef GTUT
	#undef GTUT_END
	#define GTUT_END catch(excep::base_error &e){ GTUT_ASRT(false, e.what()); }  gt::gWorld.get()->flushLines(); }
#endif

#endif

