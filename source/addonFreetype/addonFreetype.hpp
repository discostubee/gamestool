
#ifndef ADDONFREETYPE_HPP
#define ADDONFREETYPE_HPP

#if defined(__APPLE__)
#	include "gt_OSX/OSX_world.hpp"
#elif defined(__linux)
#	include "gt_linux/linux_world.hpp"
#elif defined(_WIN32)
#	include "gt_win32/win_world.hpp"
#endif

//- when using functions you plan to expose in the shared object interface, they can't be defined in the header.
DYN_LIB_EXP_DEC(void) draftAll(gt::cWorld *pWorld);
DYN_LIB_EXP_DEC(void) closeLib();

#endif
