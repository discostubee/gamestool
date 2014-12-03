#include "fridge.hpp"
#include "fileIO.hpp"
#include <fstream>

using namespace gt;

static const unsigned int MAX_ITEMS = 1000;

const dPlaChar * cFridge::FILE_PATH = "fridge.cache";

template<typename STREAM_T, typename T>
void
handyWrite(STREAM_T &stream, const T &data){
	stream.write(reinterpret_cast<const char*>(&data), sizeof(data));
}

template<typename STREAM_T, typename T>
void
handyRead(STREAM_T &stream, T * out){
	stream.read(reinterpret_cast<char*>(out), sizeof(T));
}

cFridge::cFridge(dNameHash pProgName, size_t pMaxBytes)
: maxAlloc(pMaxBytes), mName(pProgName)
{
	PROFILE;
	DBUG_VERBOSE_LO("Opening fridge.");
	try{
		std::ifstream file(FILE_PATH, std::fstream::in | std::fstream::binary);
		if(file.bad() || file.eof())
			throw std::exception();

		unsigned int ledgerHeader=0;
		handyRead(file, &ledgerHeader);
		if(ledgerHeader!=MAX_ITEMS)
			throw std::exception();

		sItemLoad item;
		std::vector<unsigned int> starts;	//- faster on swaps for ordinary data.
		std::vector<unsigned int> ends;
		ends.push_back(0); //- makes the hole loop below easier.
		for(unsigned int i=0; i < MAX_ITEMS; ++i){
			handyRead(file, &item);
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

	}catch(...){
		DBUG_LO("Writing new fridge.");

		::remove(FILE_PATH);
		std::ofstream file(FILE_PATH, std::ofstream::out | std::ofstream::binary);
		handyWrite(file, MAX_ITEMS);

		sItemLoad empty;
		empty.idProg = HASH_INVALID;
		empty.idItem = HASH_INVALID;
		empty.slice.mSize = 0;
		empty.slice.mSpot = 0;

		for(unsigned int i=0; i < MAX_ITEMS; ++i){
			handyWrite(file, empty);
			mIdxLedgerIdle.push_back(i);
		}

		long fill=0;
		for(size_t l = 0; l < maxAlloc / sizeof(long); ++l)
			handyWrite(file, fill);

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
		add.idx = pItemID;
		if(!mIdxLedgerIdle.empty()){	//- assumed holes already defragged.
			std::list<sSlice>::iterator hole = mHoles.begin();
			while(hole != mHoles.end()){
				if(hole->mSize >= pData->getSize())
					break;

				++hole;
			}

			if(hole == mHoles.end())
				return false;

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
		std::fstream file(FILE_PATH, std::fstream::out | std::fstream::in | std::fstream::binary);
		file.seekp(
			sizeof(unsigned int) + (add.idx * sizeof(sItemLoad)),
			std::fstream::beg
		);
		if(file.bad() || file.eof())
			THROW_ERROR("Bad fridge file.");

		handyWrite(file, update);

		found = mItems.insert(mItems.end(), dItems::value_type(pItemID, add));
	}

	ASRT_TRUE(found->second.slice.mSize >= pData->getSize(), "Slice must be big enough.");

	std::fstream file(FILE_PATH, std::fstream::out | std::fstream::in | std::fstream::binary);
	file.seekp(
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

	std::ifstream file(FILE_PATH, std::fstream::in | std::fstream::binary);
	file.seekg(found->second.slice.mSpot, std::fstream::beg);
	if(file.bad() || file.eof())
		THROW_ERROR("Bad fridge file.");

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

	std::fstream file(FILE_PATH, std::fstream::out | std::fstream::in | std::fstream::binary);
	file.seekp(
		sizeof(unsigned int) + (found->second.idx * sizeof(sItemLoad)),
		std::fstream::beg
	);
	handyWrite(file, empty);

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

#ifdef GTUT

GTUT_START(test_fridge, chill){
	cFridge f(makeHash("testing"), 1000);
	tAutoPtr<iArray> buff(new tArray<int>(10));
	int *arr = reinterpret_cast<int*>(buff->getBuff());
	for(int i=0; i<10; ++i)
		arr[i] = i;

	f.chill(makeHash("a"), buff);
}GTUT_END;

GTUT_START(test_fridge, thaw){
	cFridge f(makeHash("testing"), 1000);
	tAutoPtr<iArray> buff = f.thaw(makeHash("a"));
	int *arr = reinterpret_cast<int*>(buff->getBuff());
	for(size_t i=0; i<buff->getSize() / sizeof(int); ++i){
		GTUT_ASRT(arr[i] == static_cast<int>(i), "Didn't chill.");
	}
}GTUT_END;

#endif
