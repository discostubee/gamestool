#include "linux_fileIO.hpp"

using namespace gt;

cLinux_fileIO::cLinux_fileIO(){
}

cLinux_fileIO::~cLinux_fileIO(){
}

void
cLinux_fileIO::read(cByteBuffer* aOutput, const dFilePoint pStartPoint, const size_t pReadAmount){
	std::fstream stream;
	unsigned int fileSize = 0;
	char* readBuff = NULL;

	try{
		DBUG_LO("reading");
		stream.open(mPath.get().c_str(), std::fstream::in | std::fstream::binary );

		stream.seekg(0, std::fstream::end);
		fileSize = stream.tellg();

		if(fileSize == 0 || fileSize == static_cast<unsigned int>(-1) )
			THROW_ERROR("File empty");

		if( pStartPoint + pReadAmount > fileSize )
			THROW_ERROR("File too small for read amount.");

		stream.seekg(pStartPoint, std::ios_base::beg);

		if(pReadAmount == 0){	// read up to the end of the file.
			if(fileSize - pStartPoint < 1)
				THROW_ERROR("File size is smaller than the start point.");

			readBuff = new char[fileSize - pStartPoint];
			stream.read(readBuff, fileSize - pStartPoint);
			aOutput->take( static_cast<dByte*>(readBuff), fileSize - pStartPoint );
		}else{
			readBuff = new char[pReadAmount];
			stream.read(readBuff, pReadAmount);
			aOutput->take( static_cast<dByte*>(readBuff), pReadAmount );
		}

		if(stream.bad())
			THROW_ERROR("Bad stream.");

	}catch(std::exception &e){
		stream.close();
		throw;	// rethrow e.
	}
	stream.close();
}

void
cLinux_fileIO::write(const cByteBuffer* pBuff){
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
cLinux_fileIO::insert(const cByteBuffer* pBuff, dFilePoint pStartPoint){
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
cLinux_fileIO::deleteFile(){
	DBUG_LO("deleting");
	::remove(mPath.get().c_str());
}

size_t
cLinux_fileIO::getFileSize(){
	std::fstream stream;
	size_t fileSize = 0;

	stream.open(mPath.get().c_str(), std::fstream::in | std::fstream::binary);
	stream.seekg(0, std::fstream::end);
	fileSize = stream.tellg();
	stream.close();
	return fileSize;
}
