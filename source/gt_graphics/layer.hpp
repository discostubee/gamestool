/*
 * !\file	layer.hpp
 * !\brief
 */

#ifndef LAYER_HPP
#define LAYER_HPP

#include "windowFrame.hpp"

namespace gt{
	//!\brief	A frame is a 2D window you can render into
	class cLayer: public cFigment, private tOutline<cLayer>{
	protected:
		cPlug<ptrFig>	mRoot;	//!< Run everything linked by this root.

		//-------------------
		virtual void setDim(dUnitPix pX, dUnitPix pY, dUnitPix pW, dUnitPix pH){ }	//!< Sets dimensions of frame.

	public:

		//- Statics and defines
		static const cPlugTag*	xPT_width;
		static const cPlugTag*	xPT_height;
		static const cPlugTag*	xPT_posX;
		static const cPlugTag*	xPT_posY;
		static const cPlugTag*	xPT_root;

		static const cCommand*	xSetDim;	//!< Command to set dimensions.
		static const cCommand*	xSetRoot;	//!< Command to set dimensions.

		enum{
			eSetDim  = cFigment::eSwitchEnd,
			eSetRoot
		};

		static const char* identify(){ return "layer"; }

		//- Standard
		cLayer();
		virtual ~cLayer();

		virtual const char* name() const { return cLayer::identify(); }	//!<

		virtual void jack(ptrLead pLead);
		virtual cByteBuffer& save();
		virtual void loadEat(cByteBuffer* pBuff);
		virtual void getLinks(std::list<ptrFig>* pOutLinks);
	};
}
#endif

