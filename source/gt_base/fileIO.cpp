#include "fileIO.hpp"

////////////////////////////////////////////////////////////
using namespace gt;

const cPlugTag*	cBase_fileIO::xPT_buffer = tOutline<cBase_fileIO>::makePlugTag("buffer");
const cPlugTag*	cBase_fileIO::xPT_startSpot = tOutline<cBase_fileIO>::makePlugTag("start spot");
const cPlugTag*	cBase_fileIO::xPT_readSize = tOutline<cBase_fileIO>::makePlugTag("read size");
const cPlugTag*	cBase_fileIO::xPT_fileSize = tOutline<cBase_fileIO>::makePlugTag("file size");

const cCommand*	cBase_fileIO::xSetPath = tOutline<cBase_fileIO>::makeCommand(
	"set path",
	cBase_fileIO::eSetPath,
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
cBase_fileIO::jack(ptrLead pLead){
	PROFILE;

	try{
		switch( pLead->mCom->getSwitch<cBase_fileIO>() ){
			case eSetPath:
				mPath = *pLead->getPiledDItr().getPlug();
				break;

			case eRead:{
				PROFILE;
				cPlug<cByteBuffer>* buff = new cPlug<cByteBuffer>;

				DBUG_LO("reading");

				try{
					buff->mD = read(
						pLead->getD(xPT_startSpot)->getMDCopy<dFilePoint>(),
						pLead->getD(xPT_readSize)->getMDCopy<size_t>()
					);
				}catch(...){//}catch(excep::notFound){
					buff->mD = read(0,0);
				}
				pLead->take( buff, xPT_buffer);

			}break;

			case eWrite:{
				PROFILE;
				write( pLead->getD(xPT_buffer)->getMDPtr< cByteBuffer >() );
			}break;

			case eInsert:{
				PROFILE;
				size_t startSpot = ENDOF_FILE;
				try{
					startSpot = pLead->getD(xPT_startSpot)->getMDCopy<size_t>();
				}catch(...){//}catch(excep::notFound){
					startSpot = ENDOF_FILE;
				}
				insert( pLead->getD(xPT_buffer)->getMDPtr< cByteBuffer >(), startSpot );
			}break;

			case eDeleteFile:
				deleteFile();
			break;

			case eGetSize:{
				PROFILE;
				mFileSize.mD = getFileSize();
				pLead->add( &mFileSize, xPT_fileSize );
			}break;

			default:
				cFigment::jack(pLead);
				break;
		}
	}catch(excep::base_error &e){
		WARN(e);
	}
}
