/*
**********************************************************************************************************
 *  Copyright (C) 2010  Stuart Bridgens
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License (version 3) as published by
 *  the Free Software Foundation.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *********************************************************************************************************
*/

//!\file	main.cpp
//!\brief	Aims to test the world's ability to load addons, print lines and profile tokens.
//!\note	You need the  OSX GL addon.

// Include the stuff we want to test.
#include <gt_base/runList.hpp>
#include <gt_OSX/OSX_world.hpp>
#include <gt_OSX/OSX_addon.hpp>
#include <gt_graphics/windowFrame.hpp>

#include <fstream>

using namespace gt;

int
main(int argc, char **argv){
	int result = EXIT_SUCCESS;
	std::cout
		<< "Running gamestool tests for OSX. "
		<< "Version 0.2. "
#ifdef GT_THREADS
		<< "Threadding. "
#endif
		<< std::endl;

	gWorld.take( new cOSXWorld() );

	tOutline<cRunList>::draft();
	tOutline<cFigment>::draft();
	tOutline<cAddon>::draft();
	tOutline<cAddon_OSX>::draft();

	gWorld.get()->flushLines();
#ifdef GTUT_GOOGLE
	::testing::InitGoogleTest(&argc, argv);
	result = RUN_ALL_TESTS();
#endif

	{
		std::ofstream profileReport("profile_report.txt");
		gt::gWorld.get()->makeProfileReport(profileReport);//(std::cout);
		profileReport.close();
	}

	gt::gWorld.cleanup();

	return result;
}

#ifdef GTUT
namespace gt{
	GTUT_START(testOSX, load){
		cContext fake;
		ptrFig elements;
		tPlug<ptrFig> addonGL;
		tPlug<ptrFig> window;
		ptrLead setPath = gWorld.get()->makeLead(cAddon::xLoadAddon, fake.getSig());
		tPlug<dStr> path("X11GL");
		ptrLead addElements = gWorld.get()->makeLead(cRunList::xAdd, fake.getSig());

		elements = gWorld.get()->makeFig(getHash<cRunList>());

		addonGL.get() = gWorld.get()->makeFig(getHash<cAddon>());
		setPath->addPlug(&path, cAddon::xPT_addonName);
		addonGL.get()->jack(setPath, &fake);

		window.get() = gWorld.get()->makeFig(getHash<cWindowFrame>());

		addElements->addToPile(&addonGL);
		addElements->addToPile(&window);
		elements->jack(addElements, &fake);

		gWorld.get()->setRoot(elements);
	}GTUT_END;

	GTUT_START(testOSX, unload){
		bool addonBlueprintRemoved = false;

		gWorld.get()->setRoot(gWorld.get()->getEmptyFig());	// Will take out everything.
		try{
			gWorld.get()->getBlueprint(getHash<cAddon_OSX>());
		}catch(excep::base_error){
			DBUG_LO("removed the OSX addon blueprint.");
			addonBlueprintRemoved = true;
		}
		try{
			gWorld.get()->getBlueprint(getHash<cWindowFrame>());
		}catch(excep::base_error){
			DBUG_LO("removed the x11 window frame blueprint.");
			addonBlueprintRemoved = true;
		}
		GTUT_ASRT(addonBlueprintRemoved, "didn't remove blueprints.");

		TRYME(gWorld.get()->getBlueprint(getHash<cAddon>()));	// Check the base addon was restored.
	}GTUT_END;
}
#endif
