/*
 * !\file	polygonMesh.hpp
 * !\brief	Contains the polygon figment as well as all its data types.
 */

#ifndef PRINTWORKS_HPP
#define PRINTWORKS_HPP

#include "stage.hpp"

namespace gt{

	//!\brief	Printworks can take a text string and creates a bitmap from it.
	class cPrintworks : public cFigment{
	public:
		typedef short dFontSize;

		static const cPlugTag *xPT_bitmap;	//!< bitmap figment as output.
		static const cPlugTag *xPT_text;
		static const cPlugTag *xPT_fontSize;
		static const cPlugTag *xPT_fontName;	//!< Takes text to find which font to use for printing.
		static const cCommand::dUID	xPrint;	//!< Takes text and all other properties and turns it into a monochrome bitmap.

		//-----------------------------
		cPrintworks();
		virtual ~cPrintworks();

		//-----------------------------
		// Standard
		GT_IDENTIFY("printworks");
		virtual dNameHash hash() const { return getHash<cPrintworks>(); }

	protected:
	};
}

#endif
