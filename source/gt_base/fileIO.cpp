#include "fileIO.hpp"

////////////////////////////////////////////////////////////
using namespace gt;

const cPlugTag*	cBase_fileIO::xPT_buffer = tOutline<cBase_fileIO>::makePlugTag("buffer");
const cPlugTag*	cBase_fileIO::xPT_startSpot = tOutline<cBase_fileIO>::makePlugTag("start spot");
const cPlugTag*	cBase_fileIO::xPT_readSize = tOutline<cBase_fileIO>::makePlugTag("read size");
const cPlugTag*	cBase_fileIO::xPT_fileSize = tOutline<cBase_fileIO>::makePlugTag("file size");
const cPlugTag*	cBase_fileIO::xPT_filePath = tOutline<cBase_fileIO>::makePlugTag("file path");

const cCommand*	cBase_fileIO::xSetPath = tOutline<cBase_fileIO>::makeCommand(
	"set path",
	cBase_fileIO::eSetPath,
	cBase_fileIO::xPT_filePath,
	NULL
);

const cCommand*	cBase_fileIO::xRead  = tOutline<cBase_fileIO>::makeCommand(
	"read",
	cBase_fileIO::eRead,
	cBase_fileIO::xPT_startSpot,
	cBase_fileIO::xPT_readSize,
	cBase_fileIO::xPT_buffer,
	NULL
);

const cCommand*	cBase_fileIO::xWrite = tOutline<cBase_fileIO>::makeCommand(
	"write",
	cBase_fileIO::eWrite,
	cBase_fileIO::xPT_buffer,
	NULL
);

const cCommand*	cBase_fileIO::xInsert = tOutline<cBase_fileIO>::makeCommand(
	"insert",
	cBase_fileIO::eInsert,
	cBase_fileIO::xPT_buffer,
	cBase_fileIO::xPT_startSpot,
	NULL
);

const cCommand*	cBase_fileIO::xDeleteFile = tOutline<cBase_fileIO>::makeCommand(
	"delete",
	cBase_fileIO::eDeleteFile,
	NULL
);

const cCommand*	cBase_fileIO::xGetSize = tOutline<cBase_fileIO>::makeCommand(
	"file size",
	cBase_fileIO::eGetSize,
	cBase_fileIO::xPT_fileSize,
	NULL
);


cBase_fileIO::cBase_fileIO(){
}

cBase_fileIO::~cBase_fileIO(){
}

void
cBase_fileIO::jack(ptrLead pLead, cContext* pCon){
	PROFILE;

	start(pCon);
	try{
		switch( pLead->mCom->getSwitch<cBase_fileIO>() ){
			case eSetPath:
				mPath = pLead->getPlug(xPT_filePath, pCon);
				break;

			case eRead:{
				PROFILE;

				DBUG_LO("reading");

				size_t readSize = 0;
				size_t readStart = 0;
				try{
					readStart = pLead->getPlug(xPT_readSize, pCon)->getCopy<dFilePoint>();
				}catch(excep::base_error){}
				try{
					readSize = pLead->getPlug(xPT_startSpot, pCon)->getCopy<dFilePoint>();
				}catch(excep::base_error){}

				*pLead->getPlug(xPT_buffer, pCon) = read( readSize, readStart );

			}break;

			case eWrite:{
				PROFILE;
				write( pLead->getPlug(xPT_buffer, pCon)->getPtr<cByteBuffer>() );
			}break;

			case eInsert:{
				PROFILE;

				size_t startSpot = 0;
				try{
					startSpot = pLead->getPlug(xPT_startSpot, pCon)->getCopy<dFilePoint>();
				}catch(excep::base_error){}

				insert( pLead->getPlug(xPT_buffer, pCon)->getPtr<cByteBuffer>(), startSpot );
			}break;

			case eDeleteFile:
				deleteFile();
			break;

			case eGetSize:{
				PROFILE;
				mFileSize = getFileSize();
				pLead->setPlug(&mFileSize, xPT_fileSize, pCon);
			}break;

			default:
				cFigment::jack(pLead, pCon);
				break;
		}
	}catch(excep::base_error &e){
		WARN(e);
	}
	stop(pCon);
}
