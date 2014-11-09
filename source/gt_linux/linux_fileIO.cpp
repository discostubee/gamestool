#include "linux_fileIO.hpp"
#include <fstream>

using namespace gt;

cLinux_fileIO::cLinux_fileIO(){
}

cLinux_fileIO::~cLinux_fileIO(){
}

void
cLinux_fileIO::read(cByteBuffer* aOutput, const dFilePoint pStartPoint, const size_t pReadAmount){
	unsigned int fileSize = 0;
	char* readBuff = NULL;

	DBUG_VERBOSE_LO("Reading from " << mPath.get());
	std::fstream stream(mPath.get().c_str(), std::fstream::in | std::fstream::binary ); //- closed on destruction

	stream.seekg(0, std::fstream::end);
	fileSize = stream.tellg();

	if(fileSize == 0 || fileSize == static_cast<unsigned int>(-1) )
		THROW_ERROR(mPath.get() << " File is empty");

	if( pStartPoint + pReadAmount > fileSize )
		THROW_ERROR(mPath.get() << " File is too small for read amount of " << pReadAmount);

	stream.seekg(pStartPoint, std::ios_base::beg);

	readBuff = new char[pReadAmount];
	aOutput->take( static_cast<dByte*>(readBuff), pReadAmount );
	stream.read(readBuff, pReadAmount);
}

void
cLinux_fileIO::write(const cByteBuffer* pBuff){
	ASRT_NOTNULL(pBuff);

	DBUG_VERBOSE_LO("Writing to " << mPath.get());
	std::fstream stream(
		mPath.get().c_str(),
		std::fstream::out | std::fstream::binary | std::fstream::trunc
	);
	stream.write( pBuff->get(), pBuff->size() );

	if(stream.bad())
		THROW_ERROR(mPath.get() << " is a bad stream.");
}

void
cLinux_fileIO::insert(const cByteBuffer* pBuff, dFilePoint pStartPoint){
	DBUG_VERBOSE_LO("Inserting into " << mPath.get());

	std::fstream stream(mPath.get().c_str(), std::fstream::out | std::fstream::in | std::fstream::binary | std::fstream::app );
	stream.seekg(0, std::fstream::end);

	size_t fileSize = stream.tellg();
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
		THROW_ERROR(mPath.get() << " is a bad stream.");

}

void
cLinux_fileIO::deleteFile(){
	DBUG_VERBOSE_LO("Deleting file " << mPath.get());
	::remove(mPath.get().c_str());
}

size_t
cLinux_fileIO::getFileSize(){
	size_t fileSize = 0;

	std::fstream stream(mPath.get().c_str(), std::fstream::in | std::fstream::binary);
	stream.seekg(0, std::fstream::end);
	fileSize = stream.tellg();
	stream.close();
	return fileSize;
}
