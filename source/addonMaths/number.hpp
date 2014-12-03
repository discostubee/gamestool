/*
 * !\file	polygonMesh.hpp
 * !\brief	Contains the polygon figment as well as all its data types.
 */

#ifndef PRINTWORKS_HPP
#define PRINTWORKSS_HPP

#include "gt_base/figment.hpp"

namespace gt{

	//!\brief
	class cNumber : public cFigment{
	public:

		//-----------------------------
		cNumber();
		virtual ~cNumber();

		//-----------------------------
		// Standard
		GT_IDENTIFY("number");
		GT_REPLACES(cFigment);
		virtual dNameHash hash() const { return getHash<cNumber>(); }

	protected:
	};
}

#endif
