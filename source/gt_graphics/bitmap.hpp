/*
 * !\file	polygonMesh.hpp
 * !\brief	Contains the polygon figment as well as all its data types.
 */

#ifndef BITMAP_HPP
#define BITMAP_HPP

#include "gt_base/figment.hpp"

namespace gt{

	//!\brief	The bitmap abstraction uses streams for each colour channel. The implementation of this class can then take
	//!			these streams and convert them into any native format they need.
	class cBitmap : public cFigment{
	public:
		enum eChan{
			eChanRed, eChanGreen, eChanBlue, eChanAlpha, eChanMono
		};

		typedef unsigned short dColRng; //!< Colour range from 0 to its maximum value representing the maximum value possible for this bitmap type.
		typedef unsigned int dUnitPix;	//!< unit in pixels.
		typedef std::vector<dColRng> dStreamChannel;	//!<
		typedef std::map<eChan, dStreamChannel> dChannels;

		//!\brief When adding a bitmap, you can change how it's added, both in which channels to swap and how the levels are copied.
		class cBase_ChanRelation{
		public:
			eChan inChan, outChan;
			virtual void relate(dStreamChannel &in, size_t startInPos, dStreamChannel &out, size_t startOutPos) =0;	//!< copy one stream into another.
		};

		static const size_t COLRNG_MAX = USHRT_MAX;

		static const cPlugTag *xPT_bitmap;
		static const cPlugTag *xPT_addSpot;	//!< Where to put the top left corner of the bitmap we are adding. Relative to the parent's top left corner.
		static const cCommand::dUID xSetRelation;	//!< Sets channel relations which are used when adding. Adds them as a pile.
		static const cCommand::dUID	xAdd;	//!< Adds another bitmap to this one. You can place this bitmap anywhere on the parent, relative to the parent's top left corner.

		//-----------------------------
		// Standard
		cBitmap();
		virtual ~cBitmap();

		static const dPlaChar* identify(){ return "bitmap"; }
		virtual const dPlaChar* name() const{ return identify(); }		//!< Virtual version of identify.
		virtual dNameHash hash() const { return getHash<cBitmap>(); }

	protected:
		typedef std::auto_ptr<dChannels> ptrBitmap;

		//!\brief Kinky.
		struct sDirtyRect{
			dUnitPix x, y, width;
			ptrBitmap bitmap;
			sDirtyRect(const sDirtyRect &copyMe) : x(copyMe.x), y(copyMe.y), width(copyMe.width) {}
			sDirtyRect() {}
		};

		typedef std::vector< sDirtyRect > dListDirty;
		typedef tPMorphJar<cBase_ChanRelation> jarRelation;
		typedef std::map< eChan, jarRelation > dRelations;

		dListDirty mPendingAdds;		//!< List of dirty rectangles, ooo, that need to be copied into this bitmap.
		dRelations channelRelation;		//!< How channels from each dirty rect are copied into this bitmap.

		virtual ptrBitmap getBitmap(){ DONT_USE_THIS; };	//!< The implementation converts its native format into the portable channel streams format and outputs it.

		void patSetRelation(ptrLead aLead);
		void patAdd(ptrLead aLead);
	};
}

#endif
