/*
 * !\file
 * !\brief
 */

#ifndef TERMINAL_WORLD_HPP
#define TERMINAL_WORLD_HPP

#if defined(__APPLE__)
#	include "gt_OSX/OSX_world.hpp"
#elif defined(__linux)
#	include "gt_linux/linux_world.hpp"
#elif defined(_WIN32)
#	include "gt_win32/win_world.hpp"
#endif

namespace gt{

	//--------------------------------------------------------
	//!\brief	Overrides the normal 'sandbox' world to output
	//!			to the terminal instead.
	class cTerminalWorld: public
#if defined(__APPLE__)
		cOSXWorld
#elif defined(__linux)
		cLinuxWorld
#elif defined(_WIN32)
		cWinWorld
#endif
	{
	public:
		cTerminalWorld();
		virtual ~cTerminalWorld();

		virtual void flushLines	();

	protected:
		tAutoPtr<cWorld> makeWorld();
	};
}

#endif
