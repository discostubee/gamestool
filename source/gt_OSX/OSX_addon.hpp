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

#include <gt_base/addon.hpp>
#include <dlfcn.h>	// MUST include the dl library in whatever binary you build.


namespace gt{

	class cAddon_OSX: public cAddon, private tOutline<cAddon_OSX>{
	private:
		void* mLibHandle;
		std::string mLibPath;

	protected:
		virtual void draftAddon(const std::string &pName);
		virtual void closeAddon();

	public:

		static const char* identify(){ return "OSX addon"; }
		static dNameHash replaces(){ return getHash<cAddon>(); }

		cAddon_OSX();
		virtual ~cAddon_OSX();

		virtual dNameHash getReplacement() const{ return replaces(); }
	};
}

#endif
