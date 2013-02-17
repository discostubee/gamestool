/*
 * !\file
 * !\brief
 */

#ifndef TERMINAL_WORLD_HPP
#define TERMINAL_WORLD_HPP

#if defined(__APPLE__)
#	include "gt_OSX/OSX_world.hpp"
#else defined(_WIN32)
#	include "gt_win7/win_world.hpp"
#endif

namespace gt{

	//--------------------------------------------------------
	//!\brief	The Apple Mac version of the abstract world class.
	class cTerminalWorld:
#if defined(__APPLE__)
		public cOSXWorld
#elif defined(_WIN32)
		public cWinWorld
#endif
	{
	public:
		cTerminalWorld();
		virtual ~cTerminalWorld();

		virtual void flushLines();
	};
}

#endif
