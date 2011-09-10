/*
 * !\file	layer.hpp
 * !\brief
 */

#ifndef LAYER_HPP
#define LAYER_HPP

#include "windowFrame.hpp"

namespace gt{
	//!\brief	A layer is a 2D portion of a window you can render into. A stack of layers can also be blended together. You are not required to
	//!			use a layer between a window frame and a camera.
	//!\note	Layers can be either fixed to the top left corner of their parent window or layer. Or they can scale around the window centre.
	//!\note	Like other objects, we don't store any data so that our implementations are free to store things in a data format that is most
	//!			efficient for them.
	class cLayer: public cFigment, private tOutline<cLayer>{
	public:

		//- Statics and defines
		static const cPlugTag*	xPT_content;
		static const cPlugTag*	xPT_size;
		static const cPlugTag*	xPT_point;
		static const cPlugTag*	xPT_rectangle;
		static const cPlugTag*	xPT_arrangement;	//!< How the layer is arranged.
		static const cPlugTag*	xPT_cropStyle;		//!<

		static const cCommand*	xLinkContent;	//!<
		static const cCommand*	xSetSize;		//!< The size of the layer in pixels. Taken from the top left to the bottom right.
		static const cCommand*	xSetPos;		//!< Sets the position of the layer, needs a point and an arrangement type.
		static const cCommand*	xSetCrop;		//!< Sets up cropping, requires a style and rectangle. Cropping doesn't effect where this layers top, left most corner is.

		enum{
			eLinkContent = cFigment::eSwitchEnd + 1,
			eSetSize,
			eSetPos,
			eSetCrop,
			eSwitchEnd
		};

		//!\todo	This is the first class where enums are being used to set internal states. This is a problem because they can't be easily used
		//!			by the eventual visual language. Also, these enums won't be visibile to the base engine because they are part of an addon.
		enum eArrange{
			eFixedTopLeft,	//!< 0,0 location is take from the top left. Position is taken as pixels, but uses virtual distance.
			eScaleCentre	//!< 0,0 location is take from the centre. Position is taken as a percentage
		};

		enum eCropStyle{
			eFromParent,	//!< As a percentage away from the parents borders.
			eFromSelf		//!< As a number of pixels from the centre of itself.
		};

		//--- Required stuff
		static const char* identify(){ return "layer"; }
		virtual const dNatChar* name() const { return identify(); }		//!< Virtual version of identify.
		virtual dNameHash hash() const { return tOutline<cLayer>::hash(); }


		//--- Standard
		cLayer();
		virtual ~cLayer();

		virtual void jack(ptrLead pLead, cContext *pCon);
		virtual void save(cByteBuffer* pAddHere);
		virtual void loadEat(cByteBuffer *pBuff, dReloadMap *pReloads);
		virtual void getLinks(std::list<ptrFig> *pOutLinks);

	protected:
		tPlug<ptrFig>		mLink;			//!< Run everything linked by this root.
		tPlug<eArrange>		mArrangement;	//!< It's OK to have this here, because it's not a unit.
		tPlug<eCropStyle>	mCropStyle;		//!< Same as arrangement.

		//-------------------
		virtual void setSize(const sWH<dUnitVDis> &size){ DUMB_REF_ARG(size); }					//!< Sets the size of the layer.
		virtual void setPos(const sPoint2D<dUnitVDis> &pos){ DUMB_REF_ARG(pos); }							//!< Sets the position.
		virtual void setCrop(const shape::rectangle<dUnitPix32> &area){ DUMB_REF_ARG(area); }	//!< Sets the cropping dimensions.
		virtual sWH<dUnitVDis>& getSize(){ DONT_USE_THIS; }
		virtual sPoint2D<dUnitVDis>& getPos(){ DONT_USE_THIS; }
		virtual shape::rectangle<dUnitPix32>& getCrop(){ DONT_USE_THIS; }

	};
}
#endif

