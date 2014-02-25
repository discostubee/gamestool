/*
 *!\file	iFigment.hpp
 *!\brief
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
 */

#ifndef	IFIGMENT_HPP
#define IFIGMENT_HPP

#include "dirPtr.hpp"
#include "gt_string.hpp"
#include "byteBuffer.hpp"
#include "ptrTools.hpp"
#include "utils.hpp"

#include <list>
#include <boost/shared_ptr.hpp>

////////////////////////////////////////////////////////////////////
// forward declarations
namespace gt{
	class ptrFig;
	class cReload;
	class cBase_plug;
	class cPlugTag;
	class cLead;
	class cBlueprint;
	class cCommand;
	class cContext;
}

////////////////////////////////////////////////////////////////////
// Typedefs
namespace gt{
	typedef
#if defined(__APPLE__)
	unsigned long long
#elif defined(__linux)
	unsigned long long
#elif defined(WIN32)
	INT64
#endif
		dFigSaveSig;	//!< This is used to uniquely identify a figment at save and load time. Should be enough room for 64 bit memory locations.

	typedef boost::shared_ptr<cLead> ptrLead;	//!<
	typedef std::map<dFigSaveSig, cReload*> dReloadMap;

}

////////////////////////////////////////////////////////////////////
// Classes
namespace gt{

	//-------------------------------------------------------------------------------------
	//!\brief	Figment interface, put here so we have a complete interface for the ptrFig type. Refer to the implementations of this
	//!			class to get the low down on what all these methods mean (cFigContext, cFigment). Refer to the cFigment implementation
	//!			for more info such as thread safety.
	class iFigment{
	public:
		typedef tPMorphJar<cBase_plug> dPlugHolder;
		typedef unsigned short dNumVer;	//!< Version number.
		typedef std::vector<dPlugHolder> dVersionPlugs;	//!< Vector of all the different plugs in a given version.
		typedef std::vector<dVersionPlugs> dMigrationPattern;	//!< This forms a pattern to save and load from, which hopefully will allow this version of a figment to load from old data.

		virtual ~iFigment() {}
		virtual const dPlaChar* name() const =0;
		virtual dNameHash hash() const =0;

		virtual void jack(ptrLead pLead, cContext* pCon)=0;
		virtual void run(cContext* pCon)=0;
		virtual void work(cContext* pCon)=0;
		virtual void getLinks(std::list<ptrFig>* pOutLinks)=0;
		virtual void start(cContext *con)=0;
		virtual void stop(cContext *con)=0;
		virtual void save(cByteBuffer* pSaveHere) =0;
		virtual void loadEat(cByteBuffer* pLoadFrom, dReloadMap *aReloads = NULL) =0;
		virtual dMigrationPattern getLoadPattern() =0;
		virtual ptrFig getSmart() =0;	//!< Figments are cleaned up using smart pointers, so the only way to hand out references to yourself is to use this function.

		//static dNameHash replaces()	// You will need these static class in your figment if you replace.
		virtual dNameHash getReplacement() const =0;

		//static dNameHash extends()	// You will need this static class in your figment if you extend.
		virtual dNameHash getExtension() const =0;

		//static dNumVer version()	// same as extends and replaces.
		virtual dNumVer getVersion() const =0;

	protected:
		virtual void ini(cBlueprint *pBlue, tDirector<iFigment> *pSelf) =0;

	friend class cBlueprint;
	};

	//-------------------------------------------------------------------------------------
	//!\brief	Can be used as a clone sample, where it no longer cleans
	//!			up director or counts towards the total.
	class ptrFig : public tDirPtr<iFigment>{
	public:
		ptrFig();	//!< Starts without a link.
		ptrFig(iFigment* pFig);	//!< Creates a new director and passes it the content.
		ptrFig(const ptrFig &pPtr);
		~ptrFig();

		ptrFig& operator = (ptrFig const &pPtr);				//!< creates another link to the director.
		bool operator == (ptrFig const &pPtr) const;		//!< Compares memory address to see if two pointers are pointing at the same thing.
		bool operator != (ptrFig const &pPtr) const;		//!< Same.

	protected:

		//- gives access to director.
		void linkDir(tDirector<iFigment> *aDirector);
		tDirector<iFigment> *getDir();

		friend class cFigment;
		friend class cBlueprint;
	};


	//-------------------------------------------------------------------------------------
	//!\brief	An individual entry for a figment.
	class cReload{
	public:
		ptrFig		fig;
		cByteBuffer	data;	//!< Accompanying reload data. Applied after all figments have been remade.

		cReload();

		cReload(ptrFig pFig);

		//!\brief	Stores a figment before it has been passed the rest of its reloaded data. This is so that figment plugs
		//!			can see all the reloaded figments when they are passed the buffer.
		//!\param	pFig	Reference to the newly made figment (it's not reloaded until it has been passed the buffer.
		//!\param	copyMe	This is the accompanying reload buffer which has yet to be applied to the figment. The data
		//!					being pointed to is copied so that it exists at the right time.
		cReload(ptrFig pFig, const dByte* copyMe, size_t buffSize);

		//!\brief	Cleans up the data it has copied.
		~cReload();
	};

}

#endif

