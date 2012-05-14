
#ifndef ADDONWIN32GL_HPP
#define ADDONWIN32GL_HPP

#include <gt_base/addon.hpp>

//- when using functions you plan to expose in the shared object interface, they can't be defined in the header.
DYN_LIB_EXP_DEC(void) draftAll(gt::cWorld* pWorld);
DYN_LIB_EXP_DEC(void) closeLib();

#endif
