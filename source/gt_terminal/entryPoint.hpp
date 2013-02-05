/*
 * !\file	entryPoint.hpp
 * !\brief	Include this and use the macro to define the entry function for your program.
 */

#ifndef ENTRYPOINT_HPP
#define ENTRYPOINT_HPP

#ifdef __APPLE__
#	include "gt_OSX/OSX_world.hpp"
#endif

//!\brief	Terminal entry point. Pretty much every platform has the same type of entry point for terminal programs.
#define ENTRYPOINT int main(int argc, char **argv)

#endif
