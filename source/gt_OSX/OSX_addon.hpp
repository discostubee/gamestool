/*
 * !\file	OSX_addon.hpp
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
 */

#ifndef OSX_ADDON_HPP
#define OSX_ADDON_HPP

#include "gt_base/addon.hpp"

#include <dlfcn.h>	// MUST include the dl library in whatever binary you build.


namespace gt{

	class cAddon_OSX: public cAddon, private tOutline<cAddon_OSX>{
	public:

		cAddon_OSX();
		virtual ~cAddon_OSX();

		static const dPlaChar* identify(){ return "OSX addon"; }
		virtual const dPlaChar* name() const { return identify(); }		//!< Virtual version of identify.

		virtual dNameHash hash() const { return tOutline<cAddon_OSX>::hash(); }

		static dNameHash replaces(){ return getHash<cAddon>(); }

	protected:
			typedef void (*draftFoo)(cWorld *pWorld);

			virtual void draftAddon(const dText &refAddonName);
			virtual void closeAddon();

	private:
			void* mLibHandle;
			std::string mLibPath;
	};
}

#endif
