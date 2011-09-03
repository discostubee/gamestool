
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
			ptrLead loadAddon = gWorld.get()->makeLead(getHash<cAddon>(), cAddon::xLoadAddon->mID, &fake);
			tPlug<dStr> addonName;

		#if		defined	__APPLE__
			addonName.mD = "X11GL";
		#elif	defined	LINUX
			addonName.mD = "X11GL";
		#elif	defined	WIN32
			addonName.mD = "WinGL";
		#endif

			loadAddon->add(&addonName, cAddon::xPT_addonName, &fake);
			prettyAddon->jack(loadAddon, &fake);
		}
		{
			tPlug<ptrFig> stuff = gWorld.get()->makeFig(makeHash("run list"));
			tPlug<ptrFig> shiney = gWorld.get()->makeFig(makeHash("window frame"));
			tPlug<ptrFig> mesh = gWorld.get()->makeFig(makeHash("polygon mesh"));

			{
				tPlug<ptrFig> partyPooper = gWorld.get()->makeFig(makeHash("world shutoff"));
				ptrLead setCloser = gWorld.get()->makeLead(makeHash("window frame"), makeHash("link closer"), &fake);

				setCloser->add(
					&partyPooper,
					gWorld.get()->getPlugTag(makeHash("window frame"), makeHash("closer")),
					&fake
				);

				shiney.mD->jack(setCloser, &fake);
			}
			{
				tPlug<dUnitPix> width, height;
				ptrLead setWinDim = gWorld.get()->makeLead(makeHash("window frame"), makeHash("set dimensions"), &fake);

				width = 300; height = 300;

				setWinDim->add(
					&width,
					gWorld.get()->getPlugTag(makeHash("window frame"), makeHash("width")),
					&fake
				);
				setWinDim->add(
					&height,
					gWorld.get()->getPlugTag(makeHash("window frame"), makeHash("height")),
					&fake
				);

				shiney.mD->jack(setWinDim, &fake);

			}
			{
				ptrLead addStuff = gWorld.get()->makeLead(getHash<cRunList>(), cRunList::xAdd->mID, &fake);

				addStuff->addToPile(&shiney, &fake);

				stuff.mD->jack(addStuff, &fake);
			}
			{
				ptrLead vertData = gWorld.get()->makeLead(makeHash("polygon mesh"), makeHash("add vertex"), &fake);
				ptrLead polyData = gWorld.get()->makeLead(makeHash("polygon mesh"), makeHash("add polygon"), &fake);
				tPlug<sVertex> a(sVertex( 0.0f,  0.5f, 0.0f));
				tPlug<sVertex> b(sVertex(-0.5f, -0.5f, 0.0f));
				tPlug<sVertex> c(sVertex( 0.5f, -0.5f, 0.0f));
				tPlug<sPoly> front(sPoly(0, 1, 2));
				tPlug<sPoly> back(sPoly(2, 1, 0));

				vertData->addToPile(&a, &fake);
				vertData->addToPile(&b, &fake);
				vertData->addToPile(&c, &fake);
				polyData->addToPile(&front, &fake);
				polyData->addToPile(&back, &fake);

				mesh.mD->jack(vertData, &fake);
				mesh.mD->jack(polyData, &fake);
			}
			{
				ptrLead addMeshToWindow = gWorld.get()->makeLead(makeHash("window frame"), makeHash("link content"), &fake);

				addMeshToWindow->add(&mesh, makeHash("content"), &fake);

				shiney.mD->jack(addMeshToWindow, &fake);
			}

			gWorld.get()->setRoot(stuff.mD);
			gWorld.get()->loop();
		}
		gWorld.get()->setRoot(gWorld.get()->getEmptyFig());
	}
}

}

#endif
