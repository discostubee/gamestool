/*
 * !\file	selectah.hpp
 * !\brief
 *
 *  Copyright (C) 2010  Stuart Bridgens
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
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

#ifndef SELECTAH_HPP
#define SELECTAH_HPP

#include "lead.hpp"


///////////////////////////////////////////////////////////////////////////////////
// Classes
namespace gt{

	//---------------------------------------------------------------------------------------------------
	//!\brief	Selectah! Not so much about the rewind, but it is about selecting modes.
	//!			Allows enumerated types to be understood by figments external to the class they're used in.
	//!			In other words, you can use enums from classes which are in an addon without including
	//!			their header. This also allows enums to be seen and labeled by things like the editor.
	class cSelectah{
	public:
		typedef unsigned int dModeID;

		struct sMode{
			dStr name;
			dModeID ID;
		};

		cSelectah();
		~cSelectah();

		dModeID makeMode(dNameHash figOwner, const dStr &aName);	//!< Adds internal refernce and ads to global store of all modes.
		void set(dModeID);
		dModeID current();	//!< Get the current mode of this selectah.
		bool has(dModeID);	//!< Does this selectah use this mode?

		static sMode &getModeFromAll(const dStr &aName);	//!< Get any mode from any selectah.
		static void cleanupAll();							//!< Cleans up the global collection.

	private:
		typedef std::map<dNameHash, dModeID> mapStrID;
		typedef std::map<dModeID, sMode*> mapIDMode;

		static mapStrID xSHLookup;	//!< String hash lookup.
		static std::vector<sMode> xAllModes;

		mapIDMode myModes;
		mapIDMode::iterator currentMode;
		mapIDMode::iterator tmpItr;

	};

}
#endif
