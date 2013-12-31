/*
 * !\file	polygonMesh.hpp
 * !\brief	Contains the polygon figment as well as all its data types.
 */

#ifndef PRINTWORKS_HPP
#define PRINTWORKSS_HPP

#include "addonFreetype.hpp"
#include "gt_graphics/printworks.hpp"

namespace gt{

	//!\brief	Printworks can take a text string and creates a bitmap from it.
	class cPrintworks_freetype : public cPrintworks, public tAddonDependent<FreetypeAddon>{
	public:

		//-----------------------------
		cPrintworks_freetype();
		virtual ~cPrintworks_freetype();

		//-----------------------------
		// Standard
		GT_IDENTIFY("printworks freetype");
		GT_REPLACES(cPrintworks);
		virtual dNameHash hash() const { return getHash<cPrintworks_freetype>(); }

	protected:
	};
}

#endif
