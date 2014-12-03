/*
 * !\file	reflection.hpp
 * !\brief	This will be the place to put the objects which allows the program perform reflective tasks.
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
 *
 */

#ifndef ALUCARD_HPP
#define ALUCARD_HPP

#include "gt_base/figment.hpp"
#include "gt_base/plugContainer.hpp"

namespace gt{
/*
	//!\brief	Reverse Dracula has a reflection. Alucard lets you form a lead by taking a command and scanning the context for objects
	//!			to use as the target of the command, or plugs to use in the command. Once made the lead jacks into the target every
	//!			time this object is run. Blah!
	//!\note	Because leads can't be saved, this figment instead rebuilds them for you.
	//!\note	Currently only works for tagged plugs, not piles.
	class cAlucard : public cFigment{
	public:
		static const cPlugTag *xPT_figHash;		//!<
		static const cPlugTag *xPT_comUID;		//!< The command id for the lead we make.
		static const cPlugTag *xPT_target;		//!< Target of the jacking.
		static const cPlugTag *xPT_plug;		//!<
		static const cPlugTag *xPT_tag;			//!<

		static const cCommand::dUID xSetCommand;	//!< The command to use when jacking into the target.
		static const cCommand::dUID xSetTarget;		//!< Uses the provided figment as the target for the jack operation.
		static const cCommand::dUID xAddPlug;		//!< Adds a specific plug to the command. Note that if this plug is not part of the same anchor file, it's unlikely to be reloaded.
		static const cCommand::dUID xSetTargetConx;	//!< Instead of using a direct target, we'll get our target from the current context.
		static const cCommand::dUID xAddPlugConx;	//!< Find a plug in the context with a given hash.

		cAlucard();
		virtual ~cAlucard();

		GT_EXTENDS(cFigment);
		GT_IDENTIFY("alucard");
		virtual dNameHash hash() const { return getHash<cAlucard>(); }

		virtual void getLinks(std::list<ptrFig>* pOutLinks);
		virtual void work(cContext* pCon);	//!< If Alucard doesn't have a target, it uses the context to see if it can one matching the hash.

	protected:

		//!\brief	Stores the info we need to go looking for a plug the next time we run.
		struct sContextPlug{
			dNameHash type;
			cPlugTag const *tag;
			tPlug<ptrFig> found;	//!< Helpful when found.

			sContextPlug(dNameHash aType, const cPlugTag *aTag) : type(aType), tag(aTag) {}
			sContextPlug() : type(0), tag(NULL) {}
		};

		struct sActualPlug{
			tPlug<ptrFig> plug;
			cPlugTag const *tag;

			sActualPlug(const cBase_plug *aPlug, const cPlugTag *aTag) : tag(aTag) { plug = aPlug; }
			sActualPlug() : tag(NULL) {}
		};

		typedef std::list< std::list<sContextPlug>::iterator > dListOfPlugsToFind;
		typedef std::list< std::list<sActualPlug>::iterator > dListOfPlugsToAdd;

		//-
		tPlugList< sContextPlug > mContextPlugs;	//!< Contains the info we need to search for plugs in the context. This gets saved.
		tPlugList< sActualPlug > mActualPlugs;	//!< List of plugs we've added to the figment. These get saved, but they may not be present when we reload.
		dListOfPlugsToFind mNewPlugsToFind;	//!< Emptied every time we run.
		dListOfPlugsToAdd mNewPlugsToAdd;	//!< Emptied every time we run.
		tPlug<ptrFig> mTarget;
		tPlug<dNameHash> mAltTargetName;	//!< If this is not 0, we'll try and get our target from the context using the type name.
		tPlug<cCommand::dUID> mCommand;
		ptrLead mLead;	//!< This is the lead we will be playing with.
		cContext mConx;	//!< This here is the context we create to use when jacking.

		//- scratch variables
		tPlugList< sContextPlug >::itr_t tmpCPlug;
		tPlugList< sContextPlug >::itr_t tmpAPlug;

		//-
		void patSetCom(ptrLead aLead);
		void patSetTarget(ptrLead aLead);
		void patAddPlug(ptrLead aLead);
		void patAddPlugConx(ptrLead aLead);
		void patSetTargetConx(ptrLead aLead);
	};
*/
}

#endif
