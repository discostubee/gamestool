/*
 * !\file	addon.hpp
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
 */

#ifndef ADDON_HPP
#define ADDON_HPP

#include "figment.hpp"

namespace gt{
	//!\class	cAddon
	//!\brief	An addon can draft figments contained in external plugins. These plugin figments are only valid as
	//!		long as the addon containing their code is valid.
	//!\note	Because there will be plenty of modules adding the same few plugins, addon figments must be
	//!		aware of what is already loaded. That way, a plugin is only loaded once. An it is only unloaded
	//!		once every addon for the same plugin is destroyed.
	//!\note	When an addon finally dies, it removes all its blueprints from the world. Removing drafts causes any
	//!		figments of that type to become empty objects (refer to world removeBlueprint).
	//!\note	Addons must not have platform specific names. If an addon isn't available for a certain OS, that's
	//!		just too bad.
	class cAddon: public cFigment, private tOutline<cAddon>{
	public:
		static const char* xDraftAllFooStr;
		static const char* xCloseAddonFooStr;

		static const cPlugTag* xPT_addonName;
		static const cCommand::dUID xLoadAddon;

		cAddon();
		virtual ~cAddon();

		static const dPlaChar* identify(){ return "addon"; }
		virtual const dPlaChar* name() const{ return identify(); }
		virtual dNameHash hash() const{ return tOutline<cAddon>::hash(); }
		static dNumVer version(){ return 1; }
		virtual dNumVer getVersion() const { return version(); }
		virtual dMigrationPattern getLoadPattern();

		virtual void run(cContext* pCon);	//!< The addon only loads during a run.

	protected:
		typedef boost::shared_ptr<dStr> ptrStr;
		typedef std::map<dNameHash, unsigned int> dTimesOpened;

		static dTimesOpened xOpenAddons;	//!< We want to track all open addons, so we use a hash of the name to search for it, and also keep track of how many figments require that addon.

		tPlug<dText> mAddonName;	//!< Stores the name of the addon we want.
		dNameHash mAddonHash;

		void patLoadAddon(ptrLead aLead);

		virtual void draftAddon(const dText &refAddonName){ DONT_USE_THIS; }	//!< draft all the blueprints contained in an external addon.
		virtual void closeAddon(){ DONT_USE_THIS; }
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
