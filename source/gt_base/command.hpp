/**********************************************************************************************************
 * !\file	command.hpp
 * !\brief	Contains both the commands and tags.
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

#ifndef COMMAND_HPP
#define COMMAND_HPP

#include "iFigment.hpp"


///////////////////////////////////////////////////////////////////////////////////
// constants
namespace gt{
	const unsigned int uNotMyBag=0;
}

///////////////////////////////////////////////////////////////////////////////////
namespace gt{

	//----------------------------------------------------------------------------------------------------------------
	//!\brief	this helps to identify what each plug is on a lead.
	class cPlugTag{
	public:
		typedef unsigned int dUID;	//!< Unique ID.

		const dStr	mName;
		const dUID	mID;

		cPlugTag(const dPlaChar* pPlugName);
		cPlugTag(const cPlugTag &pCopyMe);
		~cPlugTag();

	private:
		cPlugTag& operator = (const cPlugTag &pCopyMe);	//!< Banned.
	};

	//----------------------------------------------------------------------------
	//!\brief Contains the commands info only, allowing it to be instantiated.
	class cCommandInfo{
	public:
		typedef unsigned int dUID;	//!< unique command ID

		static const dUID noID;

		const dUID	mID;
		const dStr	mName;
		const dNameHash mParent;	//!< Used to determine which figment this command belongs too. Not using a callback from the actual command so that extending classes can make use of this command when respawned.

		cCommandInfo(
			const dUID pID,
			const char* pName,
			const dNameHash pParentHash
		);

		cCommandInfo(const cCommandInfo &copyMe);
		~cCommandInfo();
	};

	//----------------------------------------------------------------------------
	//!\brief	Provides copy-able references to commands
	class cCommandContain{
	public:
		static const cCommandInfo * DUMMY;

		cCommandInfo const *mCom;

		cCommandContain();
		cCommandContain(const cCommandContain &copyMe);
		cCommandContain(const cCommandInfo *copyMe);

		cCommandContain& operator=(const cCommandContain &copyMe);
		cCommandContain& operator=(const cCommandInfo *copyMe);
		cCommandContain& operator+=(const cCommandContain &copyMe);
		cCommandContain& operator+=(const cCommandInfo *copyMe);
	};

	//----------------------------------------------------------------------------
	//!\brief	A command defines how the generic jack interface is used by a figment.
	//!			That means linking to the callback function, defining what figment
	//!			uses it, as well as what tagged data this command uses.
	class cCommand: public cCommandInfo{
	public:
		cCommand(
			const dUID pID,
			const char* pName,
			const dNameHash pParentHash
		);

		virtual ~cCommand();

		virtual bool usesTag(const cPlugTag* pTag) const =0;	//!< Checks if it has such a tag.
		virtual void addTag(const cPlugTag* pTag) =0;
		virtual void use(iFigment *aFig, ptrLead aLead) const =0;	//!<
		virtual cCommand *respawn(dNameHash diffParent) const =0;	//!< Allows you to create a copy of this command but with a different parent.
	};

}


#endif
