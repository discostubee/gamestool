#ifndef FRIDGE_HPP
#define FRIDGE_HPP

#include "iFigment.hpp"
#include <iostream>

namespace gt{

	//!\brief	Sort of like a disk cache that is addressed by program and item ID. However, the cache file is a single one for all
	//!			programs so that you have a more predictable amount of disk useage.
	//!\note	A goal for the fridge is to maintain consistent performance as much as possible. So lengthy, periodic times should be avoided
	//!			even if overall speed is sacrificed.
	//!\note	FILE STRUCTURE
	//!			The actual file bytes are left up to the OS file I/O functions, but the general layout should be like this, and in this order:
	//!			- First an int for the total number of items that the files was formatted to store.
	//!			- MAX_ITEMS number of sItemLoad structures, known as the ledger. If their idProg is HASH_INVALID then they are idle (doesn't
	//!			  describe any part of the cache) and should be used first before overwriting another programs item.
	//!			- Each slice
	class cFridge{
	public:

		cFridge(dNameHash pProgName, size_t pMaxBytes);	//!< !\param pProgName Name of program. !\param pMaxBytes Maximum number of bytes the fridge can use on disk.
		~cFridge();

		bool chill(dNameHash pItemID, tAutoPtr<iArray> pData); //!\return true if it was able to add. Overwrites anything from a different program or from a different run if out of room. If there is nothing left to overwrite, it returns false.
		tAutoPtr<iArray> thaw(dNameHash pItemID); //!\return A valid auto pointer. Throws if the item is not found, so first use 'has' below. Array memory is backed by the fridge, so changes to data will change what's in the fridge. Thawing does not make this item part of this run.
		bool has(dNameHash pItemID) const; //!< Only works for items belonging to this program.
		void turf(dNameHash pItemID); //!< removes an item from the list and makes it a hole that can be filled.

		class sSlice{
		public:
			size_t mSpot;
			size_t mSize;
		};

		//!\brief	Format for data stored on disk as the ledger.
		struct sItemLoad{
			dNameHash idProg;
			dNameHash idItem;	//!<
			sSlice slice;
		};

		struct sLedger{
			unsigned int idx;	//!< Index in ledger array contained on disk.
			sSlice slice;
		};

	protected:
		typedef std::map<dNameHash, sLedger> dItems;

		static const dPlaChar * FILE_PATH;

		static bool holeSort(const cFridge::sSlice & a, const cFridge::sSlice & b);

		const size_t maxAlloc;
		const dNameHash mName;

		dItems mItems; //!< Only the items for this program and this run.
		std::list<unsigned int> mIdxLedgerIdle;	//!< Indexes of idle items.
		std::list<sLedger> mIdxLedgerOther; //!< Indexes of other program items that can be overwritten as needed.
		std::list<sSlice> mHoles; //!< Sorted in order of size. Holes are calculated as needed.

		void defrag();	//!< Launches a thread that combines holes and re-arranges items to make fewer, yet larger, holes.
		void updateLedger(const sLedger & led, dNameHash pItemID);
		void writeItem(size_t pSpot, tAutoPtr<iArray> pData);
		void partFillHole(const sLedger & pFill, std::list<sSlice>::iterator hole);
	};
}

#endif
