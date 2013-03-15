/*
 * !\file	linuxWorld.hpp
 * !\brief
 */

#ifndef WIN_WORLD_HPP
#define WIN_WORLD_HPP

#include <windows.h>
#include <time.h> // for gettimeofday and timeval

#include "gt_base/world.hpp"

//!\brief	Contains windows helper functions.
namespace win{
	//--------------------------------------------------------
	typedef std::basic_string<TCHAR, std::char_traits<TCHAR>> dMSStr;

	//--------------------------------------------------------
	void checkError(const char* pFile, unsigned int pLine);	//!< Checks for a windows error and throws if there is one.
	#define CHECK_WIN_ERR win::checkError(__FILE__, __LINE__);

	//!\note	Is unsafe and doesn't perform checks.
	dStr MSStrToSTLStr(const TCHAR* pString);

	dMSStr STLStrToMSStr(const dStr &pString);

	size_t MSStrLen(const TCHAR* pString);

}

namespace gt{
	dMillisec getAppTime_impl();

	//--------------------------------------------------------
	//!\class	cWinWorld
	//!\brief	Windows XP version of the abstract world class.
	class cWinWorld: public cWorld{
	public:
		static void			setWinInst(::HINSTANCE aInst);
		static ::HINSTANCE	getWinInst();

		cWinWorld();
		virtual ~cWinWorld();

		virtual void		copyWorld(cWorld* pWorld);
		virtual dMillisec	getAppTime();
		virtual void		loop();
		virtual void		flushLines();

	protected:
		static ::HINSTANCE	xInst;

		virtual	void		closeAddon(dStr &name);

	private:
		static dMillisec getWinTime();
	};
}

#endif
