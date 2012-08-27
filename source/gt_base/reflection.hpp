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

#ifndef REFLECTION_HPP
#define REFLECTION_HPP

#include "figment.hpp"

namespace gt{

	//!\brief	Plug hound will get a plug out of a figment for you. The figment it uses is one from the context, and the context is the
	//!			one you are using when you jack in to get the plug. It gets the plug by jacking into the target and using the command
	//!			it's been given.
	//!\note	Currently only works for tagged plugs.
	class cPlugHound : public cFigment{
	public:
		static const cPlugTag *xPT_contextTargetID;	//!< The target to jack into and get our plug. Expects a figment hash ID, and uses it to look for the figment in the context.
		static const cPlugTag *xPT_command;	//!< ID of the command we use to get the plug from the target.
		static const cPlugTag *xPT_tag;		//!< Used a tag ID rather than a pointer. This is the tag we use to get the plug from the target.
		static const cPlugTag *xPT_plug;		//!< The plug we give back through the lead.
		static const cCommand::dUID xGoGetIt;		//!< Go get it boy! Expects a target, a command and a tag. Searches the jack context for the target.

		static const dPlaChar* identify(){ return "plug hound"; }
		virtual const dPlaChar* name() const { return identify(); }
		virtual dNameHash hash() const { return getHash<cPlugHound>(); }

		static dNameHash extends(){ return getHash<cFigment>(); }
		virtual dNameHash getExtension() const { return extends(); }
		static dNumVer version(){ return 1; }
		virtual dNumVer getVersion() const { return version(); }
		virtual dMigrationPattern getLoadPattern();

		cPlugHound();
		virtual ~cPlugHound();

	protected:
		tPlug<cCommand::dUID> mCom;
		tPlug<cPlugTag::dUID> mTag;
		tPlug<dNameHash> mTarget;

		void patGoGetit(ptrLead aLead);
	};

	//!\brief	Opposite Dracula has a reflection. Alucard lets you form a lead by taking a command and scanning the context for some of the objects
	//!			you want to use as plugs or even targets. Once made the lead is jacked into the target every time this object is run. Bleh!
	//!\note	Because leads can't be saved, this figment instead rebuilds them for you.
	//!\note	Currently only works for tagged plugs, not piles.
	class cAlucard : public cFigment{
	public:
		static const cPlugTag *xPT_command;		//!< Expects a plug tag hash it can use to find the command in the world.
		static const cPlugTag *xPT_target;		//!< Target of the jacking.
		static const cPlugTag *xPT_plug;		//!<
		static const cPlugTag *xPT_tag;
		static const cPlugTag *xPT_contextFig;
		static const cCommand::dUID xAddCommand;	//!< The command to use when jacking into the target.
		static const cCommand::dUID xSetTarget;		//!< Uses the provided figment as the target for the jack operation.
		static const cCommand::dUID xAddPlug;		//!< Adds a specific plug. Note that if this plug is not part of the same anchor file, it's unlikely to be reloaded.
		static const cCommand::dUID xAddContextPlug;	//!< Add info to search for a plug in the context.
		static const cCommand::dUID xSetContextTarget;	//!< Instead of using a direct target, we'll get our target from the current context.

		cAlucard();
		virtual ~cAlucard();

		static const dPlaChar* identify(){ return "alucard"; }
		virtual const dPlaChar* name() const { return identify(); }
		virtual dNameHash hash() const { return getHash<cAlucard>(); }

		static dNameHash extends(){ return getHash<cFigment>(); }
		virtual dNameHash getExtension() const { return extends(); }

		virtual void save(cByteBuffer* pAddHere);		//!< Adds to the buffer, all the data needed to reload itself. It was done this way as opposed to a return auto pointer because all save operations are buffer appends.
		virtual void loadEat(cByteBuffer* pBuff, dReloadMap *aReloads = NULL);			//!< Called load-eat because the head of the buffer is consume by the load function.
		virtual void getLinks(std::list<ptrFig>* pOutLinks);
		virtual void work(cContext* pCon);

	protected:

		//!\brief	Stores the info we need to go looking for a plug the next time we run.
		struct sContextPlug{
			dNameHash type;
			cPlugTag const *tag;
			tPlug<ptrFig> found;	//!< Helpful when found.

			sContextPlug(dNameHash aType, const cPlugTag *aTag) : type(aType), tag(aTag) {}
		};

		struct sActualPlug{
			tPlug<ptrFig> plug;
			cPlugTag const *tag;

			sActualPlug(const cBase_plug *aPlug, const cPlugTag *aTag) : tag(aTag) { plug = aPlug; }
		};

		typedef std::list< std::list<sContextPlug>::iterator > dListOfPlugsToFind;
		typedef std::list< std::list<sActualPlug>::iterator > dListOfPlugsToAdd;

		std::list<sContextPlug> mContextPlugs;	//!< Contains the info we need to search for plugs in the context. This gets saved.
		std::list<sActualPlug> mActualPlugs;		//!< List of plugs we've added to the figment. These get saved, but they may not be present when we reload.
		dListOfPlugsToFind mNewPlugsToFind;	//!< Emptied every time we run.
		dListOfPlugsToAdd mNewPlugsToAdd;	//!< Emptied every time we run.
		tPlug<ptrFig> mTarget;
		tPlug<dNameHash> mAltTargetName;	//!< If this is not 0, we'll try and get our target from the context using the type name.
		tPlug<cCommand::dUID> mCommand;
		ptrLead mLead;	//!< This is the lead we will be playing with.
		cContext mConx;	//!< This here is the context we create to use when jacking.

		//- scratch variables
		std::list<sContextPlug>::iterator tmpCPlug;
		std::list<sActualPlug>::iterator tmpAPlug;

		void patAddCom(ptrLead aLead);
		void patSetTarget(ptrLead aLead);
		void patAddPlug(ptrLead aLead);
		void patAddConxPlug(ptrLead aLead);
		void patSetConxTarget(ptrLead aLead);
	};
}

#endif
