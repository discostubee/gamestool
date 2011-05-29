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

#include "demo.hpp"

#include <addonX11GL/openGL_windowFrame.hpp>
#include <gt_OSX/OSX_world.hpp>
#include <gt_base/runList.hpp>

using namespace gt;


class Controls{
public:
	Controls(){}
	~Controls(){}

};

class Setup{
private:
	cPlug<ptrFig> stuff;
	cPlug<ptrFig> window;
	cPlug<ptrFig> demo;

public:
	Setup(){}
	~Setup(){}

	void init(){
		tOutline<cFigment>::draft();
		tOutline<cWorldShutoff>::draft();
		tOutline<cRunList>::draft();
		tOutline<cWindowFrame>::draft();
		tOutline<cWindowFrame_X11GL>::draft();
		tOutline<cBulletDemo>::draft();

		{
			ptrLead addStuff = gWorld->makeLead(getHash<cRunList>(), cRunList::xAdd->mID);

			stuff.mD = gWorld->makeFig(getHash<cRunList>());
			window.mD = gWorld->makeFig(getHash<cWindowFrame>());

			addStuff->addToPile(&stuff); addStuff->addToPile(&window);
			stuff.mD->jack(addStuff);
		}
		{
			cPlug<ptrFig> shutoff;
			ptrLead setShutoff = gWorld->makeLead(getHash<cWindowFrame>(), cWindowFrame::xSetCloser->mID);

			shutoff.mD = gWorld->makeFig(getHash<cWorldShutoff>());

			setShutoff->add(&shutoff, cWindowFrame::xPT_closer);
			window.mD->jack(setShutoff);
		}
		{
			ptrLead addWinStuff = gWorld->makeLead(getHash<cWindowFrame>(), cWindowFrame::xLinkContent->mID);

			demo = gWorld->makeFig(getHash<cBulletDemo>());
			addWinStuff->add(&demo, cWindowFrame::xPT_content);
			window.mD->jack(addWinStuff);
		}

		gWorld->setRoot(stuff.mD);
	}
};


#if defined LINUX

int
main(void){
	gWorld = new cLinuxWorld();

#elif defined __APPLE__

int
main(void){
	gWorld = new cOSXWorld();

#elif defined WIN32

int
main(
	HINSTANCE pInstance,
    HINSTANCE pPrevInst,
    LPSTR pCommandLine,
    int pWinShow
){

	gWorld = new cWinWorld(pInstance);
#endif

	try{
		DBUG_LO("Bullet physics, cloth demo.");

		Controls myControls;
		Setup mySetup;

		mySetup.init();

		gWorld->loop();

	}catch(excep::base_error &e){
		WARN(e);
	}

	SAFEDEL(gWorld);

	return 0;
}

