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

///////////////////////////////////////////////////////////////////////////////////
// Constants
namespace gt{
	const dNameHash uDoesntReplace = 0;
	const dNameHash uDoesntExtend = 0;
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
		virtual ~cBlueprint();

		virtual ptrFig make();
		virtual void unmade();
		virtual dNameHash hash() const;
		virtual const dPlaChar* name() const;
		virtual dNameHash replace() const;
		virtual dNameHash extend() const;
		virtual const cCommand* getCom(cCommand::dUID pHash) const;
		virtual const cPlugTag* getPlugTag(cPlugTag::dUID pPT) const;
		virtual dListComs getAllComs() const;
		virtual dListPTags getAllTags() const;
		virtual dExtensions getExtensions() const;
		virtual bool hasPlugTag(cPlugTag::dUID pPT) const;

		void install(
			dNameHash pHash,
			dNameHash pReplaces,
			dNameHash pExtends,
			ptrFig (*pMake)(),
			void (*pUnmade)(),
			const dPlaChar* (*pGetName)(),
			const cCommand* (*pGetCom)(cCommand::dUID),
			const cPlugTag* (*pGetPlugTag)(cPlugTag::dUID),
			dListComs (*pGetAllComs)(),
			dListPTags (*pGetAllTags)(),
			dExtensions (*pGetExtensions)(),
			bool (*pHasPlugTag)(cPlugTag::dUID),
			void (*pCleanup)()
		);

	protected:
		void (*mCleanup)();	//!< Cleans up the outline as well.

	friend class cWorld;

	private:
		dNameHash mHash;
		dNameHash mReplaces;
		dNameHash mExtends;
		ptrFig (*mMake)();
		void (*mUnmade)();
		const dPlaChar* (*mGetName)();
		const cCommand* (*mGetCom)(cCommand::dUID);
		const cPlugTag* (*mGetPlugTag)(cPlugTag::dUID);
		dListComs (*mGetAllComs)();
		dListPTags (*mGetAllTags)();
		dExtensions (*mGetExtensions)();
		bool (*mHasPlugTag)(cPlugTag::dUID);

		void iniFig(ptrFig &iniMe);
	};


}


#endif
