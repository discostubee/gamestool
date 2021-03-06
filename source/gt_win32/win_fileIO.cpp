#include "win_fileIO.hpp"

using namespace gt;

cWin_fileIO::cWin_fileIO(){
}

cWin_fileIO::~cWin_fileIO(){
}

void
cWin_fileIO::read(cByteBuffer* aOutput, const dFilePoint pStartPoint, const size_t pReadAmount){
	std::fstream stream;
	unsigned int fileSize = 0;
	char* readBuff = NULL;

	try{
		DBUG_LO("reading");
		stream.open(mPath.get().c_str(), std::fstream::in | std::fstream::binary );

		stream.seekg(0, std::fstream::end);
		fileSize = stream.tellg();

		if(fileSize == 0 || fileSize == static_cast<unsigned int>(-1) )
			throw excep::base_error("file empty", __FILE__, __LINE__); //!\todo

		if( pStartPoint + pReadAmount > fileSize )
			throw std::exception();	//!\ todo

		stream.seekg(pStartPoint, std::ios_base::beg);

		if(pReadAmount == 0){	// read up to the end of the file.
			readBuff = new char[fileSize - pStartPoint];
			stream.read(readBuff, fileSize - pStartPoint);
			aOutput->take( static_cast<dByte*>(readBuff), fileSize - pStartPoint );
		}else{
			readBuff = new char[pReadAmount];
			stream.read(readBuff, pReadAmount);
			aOutput->take( static_cast<dByte*>(readBuff), pReadAmount );
		}

		if( stream.bad() )
			throw std::exception();	//!\ todo

	}catch(std::exception &e){
		aOutput->clear();
		stream.close();
		throw;	// rethrow e.
	}
	stream.close();
}

void
cWin_fileIO::write(const cByteBuffer* pBuff){
	std::fstream stream;

	try{
		DBUG_LO("writing");
		stream.open(mPath.get().c_str(), std::fstream::out | std::fstream::binary | std::fstream::trunc );
		stream.write( pBuff->get(), pBuff->size() );

		if(stream.bad())
			throw std::exception(); //!\todo

	}catch(std::exception &e){
		stream.close();
		throw;
	}

	stream.close();
}

void
cWin_fileIO::insert(const cByteBuffer* pBuff, dFilePoint pStartPoint){
	std::fstream stream;

	try{
		size_t fileSize=0;
		DBUG_LO("inserting");

		stream.open(mPath.get().c_str(), std::fstream::out | std::fstream::in | std::fstream::binary | std::fstream::app );
		stream.seekg(0, std::fstream::end);

		fileSize = stream.tellg();

		if( fileSize == static_cast<unsigned int>(-1)
			|| fileSize == 0
			|| pStartPoint > fileSize
			|| pStartPoint == ENDOF_FILE
		){
			stream.seekp(std::fstream::end);
			stream.write( pBuff->get(), pBuff->size() );
		}else{
			char* tail = new char[fileSize - pStartPoint];
			stream.seekg(pStartPoint, std::fstream::beg);
			stream.read(tail, fileSize - pStartPoint);
			stream.seekp(pStartPoint, std::fstream::beg);
			stream.write( pBuff->get(), pBuff->size() );
			stream.write( tail, fileSize - pStartPoint );
			delete[] tail;
		}

		if(stream.bad())
			throw excep::base_error("insert went badly", __FILE__, __LINE__); //!\todo

	}catch(std::exception &e){
		stream.close();
		throw;
	}

	stream.close();
}

void
cWin_fileIO::deleteFile(){
	DBUG_LO("deleting");
	::remove(mPath.get().c_str());
}

size_t
cWin_fileIO::getFileSize(){
	std::fstream stream;
	size_t fileSize = 0;

	stream.open(mPath.get().c_str(), std::fstream::in | std::fstream::binary);
	stream.seekg(0, std::fstream::end);
	fileSize = stream.tellg();
	stream.close();
	return fileSize;
}