/*
 * !\file	world.hpp
 * !\brief	
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
 * cSomething	A class name.
 * sSomething	A structure name.
 * eSomething	an enum, for both the scope and the values.
 * mSomething	Variable data stored in a class or structure, otherwise known as a member variable.
 * pSomething	A parameter passed into a function call.
 * nSomething	A name space.
 * uSomething	Constant and constant static data where the u stands for unchanging. Not to replace p when constant data is passed to a function.
 * tSomething	A template class or struct.
 * xSomething	Static data as either a class member or a function variable.
 * dSomething	A type definition.
 * SOMETHING	A pre-processor macro or value.
 * gSomething	A global variable.
 *
 */

#ifndef	WORLD_HPP
#define WORLD_HPP

#include "profiler.hpp"
#include "utils.hpp"
#include "gt_string.hpp"
#include "exceptions.hpp"

#include <sstream>
#include <vector>
#include <list>
#include <map>
#include <set>
#include <boost/smart_ptr.hpp>

////////////////////////////////////////////////////////////////////
// Forward declarations & typedefs
namespace gt{
	class cFigment;
	class cLead;
	class cBlueprint;
	class cCommand;
	class cBase_plug;
	class cPlugTag;	

	typedef boost::shared_ptr<cFigment> ptrFig;	//!<
	typedef boost::shared_ptr<cLead> ptrLead;	//!<
	typedef const cFigment* dFigSaveSig;	//!< This is used to uniquely identify a figment at save and load time.
}


////////////////////////////////////////////////////////////////////
// Classes
namespace gt{

	//---------------------------------------------------------------------------------------------------
	//!\class	cWorld
	//!\brief	The world is a single object that ties the program together, as well as being the main 
	//!			factory that creates figment-type objects. The world object is also a singleton that is 
	//!			seen by every figment-type object and offers services to them. It also designed to 
	//!			coordinate different heaps located in addons.
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
		static void lo(const dStr& pLine);

		//!\brief	logs a warning. Note that fatal errors are caught by the catch at the top of the program; there's no need for a world function to handle it.
		void warnError(excep::base_error &pE, const dNatChar* pFile, const unsigned int pLine);

		//--------------------------------------------------------
		// Regular world methods
		cWorld();
		virtual ~cWorld();

		void setRoot(ptrFig pNewRoot);

		virtual void copyWorld(cWorld* pWorld);

		//--------------------------------------------------------
		// Blueprint stuff

		//!\brief	Adds a blueprint to the library. Will replace a blueprint if the new one say to.
		void addBlueprint(const cBlueprint* pAddMe);

		const cBlueprint* getBlueprint(dNameHash pNameHash);

		void removeBlueprint(const cBlueprint* pRemoveMe);

		//--------------------------------------------------------
		// Factory outlets

		//!\brief
		ptrFig makeFig(dNameHash pNameHash);

		//!\brief
		//!\param	pFigNameHash
		//!\param	pCommandID
		ptrLead makeLead(dNameHash pFigHash, dNameHash pComHash);

		void makeProfileReport(std::ostream &log);

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
		virtual void		loop		(){} //!< Enter the main program loop. Continues looping until it is told to stop.
		virtual void		flushLines	(){} //!< Process the lines to be displayed on the console.

	protected:

		//--------------------------------------------------------
		// Data which can be redirected from an addon's heap.
		static cCoolStatic<cWorld::dLines> xLines;
		static cCoolStatic<cProfiler> xProfiler;

		ptrFig mRoot;
		cProfiler* mProfiler;
		dLines* mLines;

		friend void redirectWorld(cWorld*);

	private:
		struct sBlueprintHeader;

		typedef std::map<dNameHash, sBlueprintHeader> dBlueprintMap;

		dBlueprintMap mBlueprints; //!< Blueprint library is static because we only every want 1 blueprint library.
		dBlueprintMap mBlueArchive; //!< Archives a blueprint here when it is replaced.
		std::vector<const cCommand*> mCommands;
		dBlueprintMap::iterator mScrBMapItr;	//!< scratch variable for iterating over blueprint library.
		ptrFig mVillageBicycle;	//!< Used for empty figment.
		bool mBicycleSetup;	//!< Faster than looking for it in the library every time.
	};
}

////////////////////////////////////////////////////////////////////
// Globals
namespace gt{
	extern cWorld* gWorld;
}

////////////////////////////////////////////////////////////////////
// Functions.
namespace gt{
	void redirectWorld(cWorld* pWorldNew);
}

////////////////////////////////////////////////////////////////////
// Macros
#ifdef DEBUG
	#define PROFILE	cProfiler::cToken profileToken = gt::gWorld->makeProfileToken(__FILE__, __LINE__)
	#define DBUG_LO(x) { std::stringstream ss; ss << x; gt::cWorld::lo(ss.str()); }
	
#else
	#define PROFILE
	#define DBUG_LO(x)
#endif

#if defined(DBUG_VERBOSE) && defined(DEBUG)
	#define DBUG_VERBOSE_LO(x) DBUG_LO(x)
#else
	#define DBUG_VERBOSE_LO(x)
#endif

#define WARN(x)	gt::gWorld->warnError(x, __FILE__, __LINE__)

// Handy for all those (...) catch blocks.
#define UNKNOWN_ERROR	{ excep::unknownError temp(__FILE__, __LINE__); WARN(temp); }

#endif

