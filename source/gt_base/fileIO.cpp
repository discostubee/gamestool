#include "fileIO.hpp"

////////////////////////////////////////////////////////////
using namespace gt;

const cPlugTag*	cBase_fileIO::xPT_buffer = tOutline<cBase_fileIO>::makePlugTag("buffer");
const cPlugTag*	cBase_fileIO::xPT_startSpot = tOutline<cBase_fileIO>::makePlugTag("start spot");
const cPlugTag*	cBase_fileIO::xPT_readSize = tOutline<cBase_fileIO>::makePlugTag("read size");
const cPlugTag*	cBase_fileIO::xPT_fileSize = tOutline<cBase_fileIO>::makePlugTag("file size");
const cPlugTag*	cBase_fileIO::xPT_filePath = tOutline<cBase_fileIO>::makePlugTag("file path");

const cCommand::dUID	cBase_fileIO::xSetPath = tOutline<cBase_fileIO>::makeCommand(
	"set path", &cBase_fileIO::patSetPath,
	cBase_fileIO::xPT_filePath,
	NULL
);

const cCommand::dUID	cBase_fileIO::xRead  = tOutline<cBase_fileIO>::makeCommand(
	"read", &cBase_fileIO::patRead,
	cBase_fileIO::xPT_startSpot,
	cBase_fileIO::xPT_readSize,
	cBase_fileIO::xPT_buffer,
	NULL
);

const cCommand::dUID	cBase_fileIO::xWrite = tOutline<cBase_fileIO>::makeCommand(
	"write", &cBase_fileIO::patWrite,
	cBase_fileIO::xPT_buffer,
	NULL
);

const cCommand::dUID	cBase_fileIO::xInsert = tOutline<cBase_fileIO>::makeCommand(
	"insert", &cBase_fileIO::patInsert,
	cBase_fileIO::xPT_buffer,
	cBase_fileIO::xPT_startSpot,
	NULL
);

const cCommand::dUID	cBase_fileIO::xDeleteFile = tOutline<cBase_fileIO>::makeCommand(
	"delete", &cBase_fileIO::patDelFile,
	NULL
);

const cCommand::dUID	cBase_fileIO::xGetSize = tOutline<cBase_fileIO>::makeCommand(
	"file size", &cBase_fileIO::patGetFileSize,
	cBase_fileIO::xPT_fileSize,
	NULL
);


cBase_fileIO::cBase_fileIO(){
}

cBase_fileIO::~cBase_fileIO(){
}

void
cBase_fileIO::patSetPath(cLead *aLead){
	mPath = aLead->getPlug(xPT_filePath);
}

void
cBase_fileIO::patRead(cLead *aLead){
	size_t readSize = 0;
	size_t readStart = 0;

	aLead->getValue(&readStart, xPT_readSize, true);
	aLead->getValue(&readSize, xPT_startSpot, true);

	read( aLead->getPlug(xPT_buffer)->exposePtr<cByteBuffer>(), readSize, readStart );
}

void
cBase_fileIO::patWrite(cLead *aLead){

	write( aLead->getPlug(xPT_buffer)->exposePtr<cByteBuffer>() );
}

void
cBase_fileIO::patInsert(cLead *aLead){
	size_t startSpot = 0;

	aLead->getValue(&startSpot, xPT_startSpot, true);

	insert( aLead->getPlug(xPT_buffer)->exposePtr<cByteBuffer>(), startSpot );
}

void
cBase_fileIO::patDelFile(cLead *aLead){
	deleteFile();
}

void
cBase_fileIO::patGetFileSize(cLead *aLead){
	PROFILE;
	mFileSize = getFileSize();
	aLead->setPlug(&mFileSize, xPT_fileSize);
}

