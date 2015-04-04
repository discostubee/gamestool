/*
**********************************************************************************************************
 *  Copyright (C) 2010  Stuart Bridgens
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License (version 3) as published by
 *  the Free Software Foundation.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *********************************************************************************************************
*/

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

cFigment::dMigrationPattern
cBase_fileIO::getLoadPattern(){
	dMigrationPattern pattern;
	dVersionPlugs version1;

	version1.push_back(&mPath);

	pattern.push_back(version1);
	return pattern;
}

void
cBase_fileIO::patSetPath(ptrLead aLead){
	aLead->copyPlug(&mPath, xPT_filePath);
}

void
cBase_fileIO::patRead(ptrLead aLead){
	PROFILE;

	tPlug<size_t> readSize;
	tPlug<size_t> readStart;
	tPlug<size_t> fileSize(getFileSize());

	(void)aLead->copyPlug(&readSize, xPT_readSize);
	(void)aLead->copyPlug(&readStart, xPT_startSpot);

	if(readStart.get() >= fileSize.get())
		throw excep::outOfRange(fileSize.get(), readStart.get(), __FILE__, __LINE__);

	if(readSize.get() == 0){
		DBUG_VERBOSE_LO("Getting entire file.");
		readSize = getFileSize() - readStart.get();
	}

	tPlug<ptrBuff> buff;
	aLead->copyPlug(&buff, xPT_buffer);
	read(buff.get().get(), readStart.get(), readSize.get());
}

void
cBase_fileIO::patWrite(ptrLead aLead){
	PROFILE;

	tPlug<ptrBuff> buffer;
	aLead->copyPlug(&buffer, xPT_buffer);
	write(buffer.get().get());
}

void
cBase_fileIO::patInsert(ptrLead aLead){
	PROFILE;

	tPlug<size_t> startSpot;
	tPlug<ptrBuff> current;

	aLead->copyPlug(&startSpot, xPT_startSpot);
	aLead->copyPlug(&current, xPT_buffer);
	insert(current.get().get(), startSpot.get());
}

void
cBase_fileIO::patDelFile(ptrLead aLead){
	PROFILE;
	deleteFile();
}

void
cBase_fileIO::patGetFileSize(ptrLead aLead){
	PROFILE;
	mFileSize = getFileSize();
	aLead->setPlug(&mFileSize, xPT_fileSize);
}

#ifdef GTUT
#	include "unitTestFigments.hpp"

GTUT_START(test_cBase_fileIO, test_suit){
	figmentTestSuit<cBase_fileIO>();
}GTUT_END;

#endif
