/*
 * !\file	polygonMesh.hpp
 * !\brief	Contains the polygon figment as well as all its data types.
 */

#ifndef BITMAP_HPP
#define BITMAP_HPP

#include "stage.hpp"
#include "gt_base/plugContainer.hpp"

namespace gt{

	//!\brief	Placed in the program flow to change the currently used texture.
	//!			Bitmaps can not be saved or loaded, but must instead be recreated.
	//!			Each bitmap is its own buffer of memory, even if it's a region
	//!			of shared memory (IE sprite sheet).
	class cBitmap : public cChainLink{
	public:
		enum eChan{
			eChanRed, eChanGreen, eChanBlue, eChanAlpha, eChanMono
		};

		typedef unsigned short dColRng; //!< Colour range from 0 to its maximum value representing the maximum value possible for this bitmap type.
		typedef std::vector<dColRng> dStreamChannel;	//!<
		typedef std::map<eChan, dStreamChannel> dChannels;

		static const cPlugTag *xPT_pic;		//!< The bitmap.
		static const cPlugTag *xPT_loc;		//!< Where to put the top left corner of the bitmap we are adding. Relative to the parent's top left corner.
		static const cPlugTag *xPT_fig;
		static const cCommand::dUID	xAdd;	//!< Adds another bitmap to this one. You can place this bitmap anywhere on the parent, relative to the parent's top left corner.
		static const cCommand::dUID	xLinkRebuilder;	//!< Link a figment that is called when the bitmap must be rebuilt.

		//-----------------------------
		// Standard
		cBitmap();
		virtual ~cBitmap();

		GT_IDENTIFY("bitmap");
		GT_EXTENDS(cChainLink);
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

		dListDirty mPendingAdds;	//!< List of dirty rectangles, ooo, that need to be copied into this bitmap.

		void patAdd(ptrLead aLead);
		void patLinkRebuilder(ptrLead aLead);

		virtual ptrBitmap getBitmap(){ DONT_USE_THIS; return ptrBitmap(NULL); };	//!< The implementation converts its native format into the portable channel streams format and outputs it.
		virtual void applyDirt(const dListDirty pDirt){ DONT_USE_THIS; }

	};
}

#endif
