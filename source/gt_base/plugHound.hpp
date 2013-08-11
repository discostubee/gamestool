/*
 * !\file	plugHound.hpp
 * !\brief
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
#include "plugContainer.hpp"

namespace gt{

	//!\brief	Plug hound will get a plug out of a figment for you. The figment it uses is one from the context, and the context is the
	//!			one you are using when you jack in to get the plug. It gets the plug by jacking into the target and using the command
	//!			it's been given.
	//!\note	Currently only works for tagged plugs.
	class cPlugHound : public cFigment{
	public:
		static const cPlugTag *xPT_contextTargetID;	//!< The target to jack into and get our plug. Expects a figment hash ID, and uses it to look for the figment in the context.
		static const cPlugTag *xPT_command;	//!< ID of the command we use to get the plug from the target.
		static const cPlugTag *xPT_tag;	//!< Used a tag ID rather than a pointer. This is the tag we use to get the plug from the target.
		static const cPlugTag *xPT_plug;	//!< The plug we give back through the lead.
		static const cCommand::dUID xGoGetIt;	//!< Go get it boy! Expects a target, a command and a tag. Searches the jack context for the target.

		GT_IDENTIFY("plug hound");
		GT_EXTENDS(cFigment);
		GT_VERSION(1);
		virtual dNameHash hash() const { return getHash<cPlugHound>(); }

		cPlugHound();
		virtual ~cPlugHound();

	protected:
		tPlug<cCommand::dUID> mCom;
		tPlug<cPlugTag::dUID> mTag;
		tPlug<dNameHash> mTarget;

		void patGoGetit(ptrLead aLead);
	};
}

#endif
