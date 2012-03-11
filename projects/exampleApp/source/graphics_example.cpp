
#ifndef GRAPHICS_EXAMPLE_HPP
#define GRAPHICS_EXAMPLE_HPP

#if defined(__APPLE__)
	#include <gt_OSX/OSX_addon.hpp>
#elif defined(LINUX)
	#include <gt_linux/linux_addon.hpp>
#elif defined(WIN32)
	#include <gt_win7/win_addon.hpp>
#endif

#include <gt_base/addon.hpp>
#include <gt_graphics/polygonMesh.hpp>
#include <gt_graphics/windowFrame.hpp>
#include <gt_base/runList.hpp>

namespace showoff{

//!\brief	Showing off the graphics engine. You may notice that a lot of identifiers are using strings, bleh! This is because a lot of these
//!			objects come from an addon, and normally we wouldn't be hand coding stuff like this (eventually, these programs will be written
//!			in a visual drag and drop fashion). So for now, we're using string hashes.
inline void graphics(){
	using namespace gt;

	PROFILE;
	DBUG_LO("showing off addons.");

	//- Need to draft all the objects we want to use first:
	tOutline<cFigment>::draft();
	tOutline<cEmptyFig>::draft();
	tOutline<cWorldShutoff>::draft();
	tOutline<cAddon>::draft();
	tOutline<cRunList>::draft();

#if defined(__APPLE__)
	tOutline<cAddon_OSX>::draft();
#elif defined(LINUX)
	tOutline<cAddon_linux>::draft();
#elif	defined(WIN32)
	tOutline<cAddon_win>::draft();
#endif

	{
		ptrFig prettyAddon = gWorld.get()->makeFig(getHash<cAddon>());
		cContext fake;

		{
			ptrLead loadAddon = gWorld.get()->makeLead(cAddon::xLoadAddon, fake.getSig());
			tPlug<dStr> addonName;

		#if		defined	__APPLE__
			addonName.mD = "X11GL";
		#elif	defined	LINUX
			addonName.mD = "X11GL";
		#elif	defined	WIN32
			addonName.mD = "WinGL";
		#endif

			loadAddon->addPlug(&addonName, cAddon::xPT_addonName);
			prettyAddon->jack(loadAddon, &fake);
		}
		{
			tPlug<ptrFig> stuff = gWorld.get()->makeFig(makeHash("run list"));
			tPlug<ptrFig> shiney = gWorld.get()->makeFig(makeHash("window frame"));
			tPlug<ptrFig> layer = gWorld.get()->makeFig(makeHash("layer"));
			tPlug<ptrFig> mesh = gWorld.get()->makeFig(makeHash("polygon mesh"));
			tPlug<ptrFig> drawlist = gWorld.get()->makeFig(makeHash("run list"));
			//tPlug<ptrFig> camera = gWorld.get()->makeFig(makeHash("camera"));

			{
				tPlug<ptrFig> partyPooper = gWorld.get()->makeFig(makeHash("world shutoff"));
				ptrLead setCloser = gWorld.get()->makeLead("window frame", "link closer", fake.getSig());

				setCloser->addPlug( &partyPooper, gWorld.get()->getPlugTag("window frame", "closer") );

				shiney.mD->jack(setCloser, &fake);
			}
			{
				tPlug<dUnitPix32> width, height;
				ptrLead setWinDim = gWorld.get()->makeLead("window frame", "set dimensions", fake.getSig());

				width = 300; height = 300;

				setWinDim->addPlug( &width, gWorld.get()->getPlugTag("window frame", "width") );
				setWinDim->addPlug( &height, gWorld.get()->getPlugTag("window frame", "height") );

				shiney.mD->jack(setWinDim, &fake);
			}
			{
				const cPlugTag *link = gWorld.get()->getPlugTag("window frame", "content");
				ptrLead addLayerToWindow = gWorld.get()->makeLead("window frame", "link content", fake.getSig());
				addLayerToWindow->addPlug(&layer, link);
				shiney.mD->jack(addLayerToWindow, &fake);
			}
			{
				ptrLead addStuff = gWorld.get()->makeLead(cRunList::xAdd, fake.getSig());
				addStuff->addToPile(&shiney);
				stuff.mD->jack(addStuff, &fake);
			}
			{
				ptrLead vertData = gWorld.get()->makeLead("polygon mesh", "add vertex", fake.getSig());
				ptrLead polyData = gWorld.get()->makeLead("polygon mesh", "add polygon", fake.getSig());
				tPlug<sVertex> a(sVertex( 0.0f,  0.5f, 0.0f));
				tPlug<sVertex> b(sVertex(-0.5f, -0.5f, 0.0f));
				tPlug<sVertex> c(sVertex( 0.5f, -0.5f, 0.0f));
				tPlug<sPoly> front(sPoly(0, 1, 2));
				tPlug<sPoly> back(sPoly(2, 1, 0));

				vertData->addToPile(&a);
				vertData->addToPile(&b);
				vertData->addToPile(&c);
				polyData->addToPile(&front);
				polyData->addToPile(&back);

				mesh.mD->jack(vertData, &fake);
				mesh.mD->jack(polyData, &fake);
			}
			{
				ptrLead addToList = gWorld.get()->makeLead(cRunList::xAdd, fake.getSig());
				ptrLead addToLayer = gWorld.get()->makeLead("layer", "link content", fake.getSig());
				const cPlugTag *contentTag = gWorld.get()->getPlugTag("layer", "content");

				//addToList->addToPile(&camera, &fake);
				addToList->addToPile(&mesh);
				drawlist.mD->jack(addToList, &fake);

				addToLayer->addPlug(&drawlist, contentTag);
				layer.mD->jack(addToLayer, &fake);
			}

			gWorld.get()->setRoot(stuff.mD);
			gWorld.get()->loop();
		}
		gWorld.get()->setRoot(gWorld.get()->getEmptyFig());
	}
}

}

#endif
