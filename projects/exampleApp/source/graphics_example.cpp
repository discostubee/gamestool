
#ifndef GRAPHICS_EXAMPLE_HPP
#define GRAPHICS_EXAMPLE_HPP

#if defined(__APPLE__)
	#include "gt_OSX/OSX_world.hpp"
#elif defined(LINUX)
	#include "gt_linux/linux_world.hpp"
#elif defined(WIN32)
	#include "gt_win7/win_world.hpp"
#endif

#include "gt_base/anchor.hpp"
#include "gt_graphics/polygonMesh.hpp"
#include "gt_graphics/windowFrame.hpp"
#include "gt_base/runList.hpp"

//!\brief	Showing off the graphics engine. You may notice that a lot of identifiers are using strings, bleh! This is because a lot of these
//!			objects come from an addon, and normally we wouldn't be hand coding stuff like this (eventually, these programs will be written
//!			in a visual drag and drop fashion). So for now, we're using string hashes.
inline void graphics(){
	using namespace gt;

	PROFILE;

	//- Need to draft all the objects we want to use first:
	tOutline<cFigment>::draft();
	tOutline<cEmptyFig>::draft();
	tOutline<cWorldShutoff>::draft();
	tOutline<cRunList>::draft();
	tOutline<cAnchor>::draft();

	{
		{
			cContext fake;

			tPlug<ptrFig> prettyAddon = gWorld.get()->makeFig("addon");
			tPlug<ptrFig> saver = gWorld.get()->makeFig("anchor");
			tPlug<ptrFig> stuff = gWorld.get()->makeFig("run list");
			tPlug<ptrFig> shiney = gWorld.get()->makeFig("window frame");
			tPlug<ptrFig> layer = gWorld.get()->makeFig("layer");
			tPlug<ptrFig> mesh = gWorld.get()->makeFig("polygon mesh");
			tPlug<ptrFig> drawlist = gWorld.get()->makeFig("run list");
			tPlug<ptrFig> camera = gWorld.get()->makeFig("3D camera");


			#if		defined	__APPLE__
				gWorld.get()->openAddon("X11GL");
			#elif	defined	LINUX
				gWorld.get()->openAddon("X11GL");
			#elif	defined	WIN32
				gWorld.get()->openAddon("WinGL");
			#endif

			{
				tPlug<ptrFig> partyPooper = gWorld.get()->makeFig(makeHash(toNStr("world shutoff")));
				ptrLead setCloser = gWorld.get()->makeLead("window frame", "link closer");

				setCloser->addPlug( &partyPooper, gWorld.get()->getPlugTag("window frame", "closer") );

				shiney.get()->jack(setCloser, &fake);
			}
			{
				tPlug<dUnitPix32> width, height;
				ptrLead setWinDim = gWorld.get()->makeLead("window frame", "set dimensions");

				width = 300; height = 300;

				setWinDim->addPlug( &width, gWorld.get()->getPlugTag("window frame", "width") );
				setWinDim->addPlug( &height, gWorld.get()->getPlugTag("window frame", "height") );

				shiney.get()->jack(setWinDim, &fake);
			}
			{
				const cPlugTag *link = gWorld.get()->getPlugTag("window frame", "content");
				ptrLead addLayerToWindow = gWorld.get()->makeLead("window frame", "link content");
				addLayerToWindow->addPlug(&layer, link);
				shiney.get()->jack(addLayerToWindow, &fake);
			}
			{
				ptrLead addStuff = gWorld.get()->makeLead(cRunList::xAdd);
				addStuff->addToPile(&shiney);
				stuff.get()->jack(addStuff, &fake);
			}
			{
				ptrLead vertData = gWorld.get()->makeLead("polygon mesh", "add vertex");
				ptrLead polyData = gWorld.get()->makeLead("polygon mesh", "add polygon");
				tPlug<sVertex> a(sVertex( 0.0f,  0.5f, 0.0f));
				tPlug<sVertex> b(sVertex(-0.5f, -0.5f, 0.0f));
				tPlug<sVertex> c(sVertex( 0.5f, -0.5f, 0.0f));
				tPlug<sPoly> front(sPoly(0, 1, 2));
				tPlug<sPoly> back(sPoly(2, 1, 0));

				vertData->addToPile(&a);
				vertData->addToPile(&b);
				vertData->addToPile(&c);
				mesh.get()->jack(vertData, &fake);

				polyData->addToPile(&front);
				polyData->addToPile(&back);
				mesh.get()->jack(polyData, &fake);
			}
			{
				ptrLead addToList = gWorld.get()->makeLead(cRunList::xAdd);
				ptrLead addToLayer = gWorld.get()->makeLead("layer", "link content");
				const cPlugTag *contentTag = gWorld.get()->getPlugTag("layer", "content");

				//addToList->addToPile(&camera, &fake);
				addToList->addToPile(&mesh);
				drawlist.get()->jack(addToList, &fake);

				addToLayer->addPlug(&drawlist, contentTag);
				layer.get()->jack(addToLayer, &fake);
			}
			{
				ptrLead link = gWorld.get()->makeLead(cAnchor::xSetRoot);
				cPlugTag const *linkTag = gWorld.get()->getPlugTag("anchor", "root");

				link->addPlug(&shiney, linkTag);
				saver.get()->jack(link, &fake);
			}

			gWorld.get()->setRoot(saver.get());
		}
	}
}

#endif
