/*
 * !\file		blueprint.hpp
 * !\brief		The blueprint file contains both the blueprint class and the figment outline template. This is because the 2
 * !			are pretty interdependent.
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
 */

#ifndef BLUEPRINT_HPP
#define BLUEPRINT_HPP

#include "command.hpp"

#include <list>

///////////////////////////////////////////////////////////////////////////////////
// Constants
namespace gt{
	const dNameHash uDoesntReplace = 0;
	const dNameHash uDoesntExtend = 0;
}

///////////////////////////////////////////////////////////////////////////////////
// Forward dec
namespace gt{
	class cBlueprint;
	class ptrFig;
}

///////////////////////////////////////////////////////////////////////////////////
// Types
namespace gt{
	typedef std::list<const cCommand*> dListComs;
	typedef std::list<const cPlugTag*> dListPTags;
	typedef std::vector<const cBlueprint *> dExtensions;
}

///////////////////////////////////////////////////////////////////////////////////
// Classes
namespace gt{


	//---------------------------------------------------------------------------------------------------
	//!\brief	Blueprint for a figment, used by the world when making figments and is the interface to
	//!			to the outline.
	class cBlueprint{
	public:
		cBlueprint();
		~cBlueprint();

		ptrFig make();
		dNameHash hash() const;
		const dPlaChar* name() const;
		dNameHash replace() const;
		dNameHash extend() const;
		const cCommand* getCom(cCommand::dUID pHash) const;
		const cPlugTag* getPlugTag(cPlugTag::dUID pPT) const;
		dListComs getAllComs() const;
		dListPTags getAllTags() const;
		dExtensions getExtensions() const;
		bool hasPlugTag(cPlugTag::dUID pPT) const;
		void remove();	//!< Calls the outlines remove function.

		void setup(
			dNameHash pHash,
			dNameHash pReplaces,
			dNameHash pExtends,
			ptrFig (*pMake)(),
			const dPlaChar* (*pGetName)(),
			const cCommand* (*pGetCom)(cCommand::dUID),
			const cPlugTag* (*pGetPlugTag)(cPlugTag::dUID),
			dListComs (*pGetAllComs)(),
			dListPTags (*pGetAllTags)(),
			dExtensions (*pGetExtensions)(),
			bool (*pHasPlugTag)(cPlugTag::dUID),
			void (*pRemove)()
		);

	private:
		dNameHash mHash;
		dNameHash mReplaces;
		dNameHash mExtends;
		ptrFig (*mMake)();
		const dPlaChar* (*mGetName)();
		const cCommand* (*mGetCom)(cCommand::dUID);
		const cPlugTag* (*mGetPlugTag)(cPlugTag::dUID);
		dListComs (*mGetAllComs)();
		dListPTags (*mGetAllTags)();
		dExtensions (*mGetExtensions)();
		bool (*mHasPlugTag)(cPlugTag::dUID);
		void (*mRemove)();

		void iniFig(ptrFig &iniMe);
	};


}


#endif
