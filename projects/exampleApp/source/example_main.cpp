/*
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
 */


//#define WIN32 //should be a project setting.
//#define LINUX //should be a project setting.

#include "context_example.cpp"
#include "addon_example.cpp"
#include "userProgram_example.cpp"
#include "graphics_example.cpp"

using namespace gt;

#if defined(__APPLE__)

int
main(void){
	gWorld.take( new cOSXWorld() );

#elif defined(LINUX)

int
main(void){
	gWorld = new cLinuxWorld();

#elif defined(WIN32)

int
main(
	HINSTANCE pInstance,
    HINSTANCE pPrevInst,
    LPSTR pCommandLine,
    int pWinShow
){

	gWorld.take(new cWinWorld(pInstance));
#endif

	try{
		DBUG_LO("Example collection. Version 3.");


		gWorld.get()->flushLines();
		gWorld.get()->makeProfileReport(std::cout);

	}catch(excep::base_error &e){
		WARN(e);
	}


	gWorld.cleanup();

	cTracker::makeReport(std::cout);

	return 0;
}

