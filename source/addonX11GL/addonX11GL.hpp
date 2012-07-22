
#ifndef ADDONX11GL_HPP
#define ADDONX11GL_HPP

#include "openGL_layer.hpp"
#include "openGL_polyMesh.hpp"
#include "openGL_bitmap.hpp"
#include "openGL_camera.hpp"

//- when using functions you plan to expose in the shared object interface, they can't be defined in the header.
DYN_LIB_EXP_DEC(void) draftAll(gt::cWorld *pWorld);
DYN_LIB_EXP_DEC(void) closeLib();

#endif
