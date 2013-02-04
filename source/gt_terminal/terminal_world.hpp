/*
 * !\file
 * !\brief
 */

#ifndef TERMINAL_WORLD_HPP
#define TERMINAL_WORLD_HPP

#ifdef __APPLE__
#	include "gt_OSX/OSX_world.hpp"
#endif

namespace gt{

	//--------------------------------------------------------
	//!\brief	The Apple Mac version of the abstract world class.
	class cTerminalWorld:
#ifdef __APPLE__
		public cOSXWorld
#endif
	{
	public:
		cTerminalWorld();
		virtual ~cTerminalWorld();

		virtual void flushLines();
	};
}

#endif
