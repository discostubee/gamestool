#include "fridge.hpp"
#include "fileIO.hpp"
#include <fstream>

using namespace gt;

static const unsigned int MAX_ITEMS = 1000;

const dPlaChar * cFridge::FILE_PATH = "fridge.cache";

std::istream&
operator >> (std::istream & stream, cFridge::sSlice & slice){
	stream >> slice.mSpot >> slice.mSize;
	return stream;
}

std::ostream&
operator << (std::ostream & stream, const cFridge::sSlice & slice){
	stream << slice.mSpot << slice.mSize;
	return stream;
}

std::istream&
operator >> (std::istream & stream, cFridge::sItemLoad & item){
	stream >> item.idProg >> item.idItem >> item.slice;
	return stream;
}

std::ostream&
operator << (std::ostream & stream, const cFridge::sItemLoad & item){
	stream << item.idProg << item.idItem << item.slice;
	return stream;
}

std::istream&
operator >> (std::istream & stream, cFridge::sLedger & ledge){
	stream >> ledge.idx >> ledge.slice;
	return stream;
}

std::ostream&
operator << (std::ostream & stream, const cFridge::sLedger & ledge){
	stream << ledge.idx << ledge.slice;
	return stream;
}

cFridge::cFridge(dNameHash pProgName, size_t pMaxBytes)
: maxAlloc(pMaxBytes), mName(pProgName)
{
	PROFILE;
	DBUG_VERBOSE_LO("Opening fridge.");
	try{
		std::fstream file(FILE_PATH, std::fstream::in | std::fstream::binary);
		if(file.bad())
			throw std::exception();

		unsigned int ledgerHeader=0;
		file >> ledgerHeader;
		if(ledgerHeader!=MAX_ITEMS)
			throw std::exception();

		sItemLoad item;
		std::vector<unsigned int> starts;	//- faster on swaps for ordinary data.
		std::vector<unsigned int> ends;
		ends.push_back(0); //- makes the hole loop below easier.
		for(unsigned int i=0; i < MAX_ITEMS; ++i){
			file >> item;
			if(item.idProg == HASH_INVALID){
				mIdxLedgerIdle.push_back(i);
			}
			else if(item.idProg == mName){
				sLedger add;
				add.idx  = i;
				add.slice = item.slice;
				(void)mItems.insert( dItems::value_type(item.idItem, add) );
			}
			else{
				sLedger other;
				other.idx = i;
				other.slice = item.slice;
				mIdxLedgerOther.push_back(other);
			}

			if(item.idProg != HASH_INVALID){
				starts.push_back(item.slice.mSpot);
				ends.push_back(item.slice.mSpot + item.slice.mSize);
			}
		}

		std::sort(starts.begin(), starts.end(), simpleSorts::AGreaterB<unsigned int> );
		std::sort(ends.begin(), ends.end(), simpleSorts::AGreaterB<unsigned int> );

		sSlice hole;
		for(
			std::vector<unsigned int>::iterator st=starts.begin(), ed=ends.begin();
			st != starts.end();
			++st, ++ed
		){
			hole.mSize = *ed - *st;
			if(hole.mSize > 0){
				hole.mSpot = *st;
				mHoles.push_back(hole);
			}
		}

		mHoles.sort(holeSort);

	}catch(std::exception &e){
		DBUG_LO("   Writing new file.");

		std::fstream file(FILE_PATH, std::fstream::out | std::fstream::binary);

		::remove(FILE_PATH);

		file << MAX_ITEMS;

		sItemLoad empty;
		empty.idProg = HASH_INVALID;
		empty.idItem = HASH_INVALID;
		empty.slice.mSize = 0;
		empty.slice.mSpot = 0;

		for(unsigned int i=0; i < MAX_ITEMS; ++i){
			file << empty;
			mIdxLedgerIdle.push_back(i);
		}

		for(size_t l = 0; l < maxAlloc / sizeof(long); ++l)
			file << static_cast<long>(0);

		sSlice bighole;
		bighole.mSize = maxAlloc;
		bighole.mSpot = sizeof(MAX_ITEMS) + (MAX_ITEMS * sizeof(sItemLoad));
		mHoles.push_back(bighole);
	}
}

cFridge::~cFridge(){

}

bool
cFridge::chill(dNameHash pItemID, tAutoPtr<iArray> pData){
	PROFILE;
	dItems::iterator found = mItems.find(pItemID);
	if(found == mItems.end()){
		sLedger add;
		if(!mIdxLedgerIdle.empty()){	//- assumed holes already defragged.
			std::list<sSlice>::iterator hole = mHoles.begin();
			while(hole != mHoles.end()){
				if(hole->mSize >= pData->getSize())
					break;

				++hole;
			}

			if(hole == mHoles.end())
				return false;

			add.idx = mIdxLedgerIdle.front();
			add.slice.mSpot=hole->mSpot;
			add.slice.mSize=pData->getSize();
			if(hole->mSize == pData->getSize()){
				mHoles.erase(hole);
			}
			else{
				hole->mSpot += pData->getSize();
				hole->mSize -= pData->getSize();
			}

			mIdxLedgerIdle.pop_front();
			mHoles.sort(holeSort);
		}
		else if(!mIdxLedgerOther.empty()){	//- todo better search for better fit.
			std::list<sLedger>::iterator itr=mIdxLedgerOther.begin();
			while(itr != mIdxLedgerOther.end()){
				if(pData->getSize() <= itr->slice.mSize){
					add.idx = itr->idx;
					add.slice.mSpot = itr->slice.mSpot;
					add.slice.mSize = pData->getSize();

					if(pData->getSize() < itr->slice.mSize){
						sSlice hole;
						hole.mSize = itr->slice.mSize - pData->getSize();
						hole.mSpot = itr->slice.mSpot + pData->getSize();
						mHoles.push_back(hole);
					}

					mIdxLedgerOther.erase(itr);

					break;
				}
				++itr;
			}

			//- todo combine some to make a bigger whole.
			if(itr == mIdxLedgerOther.end())
				return false;

		}
		else{
			return false;
		}

		sItemLoad update;
		update.idItem = add.idx;
		update.idProg = mName;
		update.slice = add.slice;
		std::fstream file(FILE_PATH, std::fstream::out | std::fstream::binary);
		file.seekg(
			sizeof(unsigned int) + (add.idx * sizeof(sItemLoad)),
			std::fstream::beg
		);
		file << update;

		found = mItems.insert(mItems.end(), dItems::value_type(pItemID, add));
	}

	ASRT_TRUE(found->second.slice.mSize >= pData->getSize(), "Slice must be big enough.");

	std::fstream file(FILE_PATH, std::fstream::out | std::fstream::binary);
	file.seekg(
		found->second.slice.mSpot,
		std::fstream::beg
	);
	file.write(pData->getBuff(), pData->getSize());

	return true;
}

tAutoPtr<iArray>
cFridge::thaw(dNameHash pItemID){

	PROFILE;

	dItems::iterator found = mItems.find(pItemID);
	if(found == mItems.end())
		throw excep::notFound("Fridge item", __FILE__, __LINE__);

	tAutoPtr<iArray> rtn(
		new tArray<dByte>(found->second.slice.mSize)
	);

	std::fstream file(FILE_PATH, std::fstream::in | std::fstream::binary);
	file.seekg(found->second.slice.mSpot, std::fstream::beg);
	file.read(rtn->getBuff(), rtn->getSize());
	return rtn;
}

bool
cFridge::has(dNameHash pItemID) const{
	PROFILE;
	return mItems.find(pItemID) != mItems.end();
}

void
cFridge::turf(dNameHash pItemID){
	PROFILE;

	dItems::iterator found = mItems.find(pItemID);
	if(found == mItems.end())
		throw excep::notFound("Fridge item", __FILE__, __LINE__);

	sItemLoad empty;
	empty.idItem = HASH_INVALID;
	empty.idProg = HASH_INVALID;
	empty.slice.mSpot = 0;
	empty.slice.mSize = 0;

	std::fstream file(FILE_PATH, std::fstream::out | std::fstream::binary);
	file.seekg(
		sizeof(unsigned int) + (found->second.idx * sizeof(sItemLoad)),
		std::fstream::beg
	);
	file << empty;

	mHoles.push_back(found->second.slice);
	mItems.erase(found);
}

bool
cFridge::holeSort(const cFridge::sSlice & a, const cFridge::sSlice & b){
	return a.mSize > b.mSize;
}

void
cFridge::defrag(){

}


