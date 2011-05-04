/*
 * !\file	addon.hpp
 * !\brief
 *
 *  Copyright (C) 2010  Stuart Bridgens
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License (version 3) as published by
 *  the Free Software Foundation
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#ifndef ADDON_HPP
#define ADDON_HPP

#include "figment.hpp"

namespace gt{
	//!\class	cAddon
	//!\brief	An addon can draft figments contained in external plugins. These plugin figments are only valid as
	//!		long as the addon containing their code is valid.
	//!\note	Because there will be plenty of modules adding the same few plugins, addon figments must be
	//!		aware of what is already loaded. That way, a plugin is only loaded/unloaded once.
	//!\note	When an addon dies, it removes all its blueprints from the world. Removing drafts causes any
	//!		figments of that type to become empty objects (refer to world removeBlueprint).
	class cAddon: public cFigment, private tOutline<cAddon>{
	protected:
		typedef boost::shared_ptr<dStr> ptrStr;

		static std::vector<ptrStr> xOpenAddons;	//!< Tracks all open addons. Allowing you to only open a library when you need to. Using vector because they're fast to iterate over.

		virtual void draftAddon(const dStr &pName){}	//!< draft all the blueprints contained in an external addon.
		virtual void closeAddon(){}

		ptrStr mAddonName;

	public:
		static const char* xDraftAllFooStr;
		static const char* xCloseAddonFooStr;

		static const cPlugTag* xPT_addonName;
		static const cCommand* xLoadAddon;

		enum{
			eLoadAddon = cFigment::eSwitchEnd +1,
			eSwitchEnd
		};

		static const dNatChar* identify(){ return "addon"; }
		static void requirements();

		cAddon();
		virtual ~cAddon();

		virtual const dNatChar* name() const{ return identify(); }
		virtual dNameHash hash() const{ return tOutline<cAddon>::hash(); }

		virtual void jack(ptrLead pLead);
	};

	
	//--------------------------------------------------------
	// Useful for making addon functions easy to implement.
	#ifdef WIN32
		//#define DYN_LIB_IMP_DEC(rnt) extern "C" __declspec(dllimport) rnt __stdcall
		#define DYN_LIB_EXP_DEC(rnt) extern "C" __declspec(dllexport) rnt
		#define DYN_LIB_DEF(rnt) __declspec(dllexport) rnt
	#else
		//#define DYN_LIB_IMP_DEC(rnt) extern "C" rnt __stdcall
		#define DYN_LIB_EXP_DEC(rnt) extern "C" rnt
		#define DYN_LIB_DEF(rnt) rnt
	#endif

}


#endif
