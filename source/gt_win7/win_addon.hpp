/*
 * !\file	win_addon.hpp
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

#ifndef WIN_ADDON_HPP
#define WIN_ADDON_HPP

#include <gt_base/addon.hpp>
#include "win_world.hpp"

namespace gt{

	class cAddon_win: public cAddon, private tOutline<cAddon_win>{
	private:
		typedef void (*dPtrDraftAll)(cWorld*);
		typedef void (*dPtrCloseLib)(void);

		dStr mLibPath;
		HINSTANCE mLibHand;

	protected:
		virtual void draftAddon(const dStr &pName);
		virtual void closeAddon();

	public:

		static const dNatChar* identify(){ return "win addon"; }
		static dNameHash replaces(){ return getHash<cAddon>(); }
		static void requirements();

		cAddon_win();
		virtual ~cAddon_win();
	};
}

#endif
