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
	addToUpdateRoster(&mPath);
	addToUpdateRoster(&mFileSize);
}

cBase_fileIO::~cBase_fileIO(){
}

void
cBase_fileIO::patSetPath(ptrLead aLead){
	mPath = aLead->getPlug(xPT_filePath);
}

void
cBase_fileIO::patRead(ptrLead aLead){
	size_t readSize = 0;
	size_t readStart = 0;

	aLead->getValue(&readStart, xPT_readSize, true);
	aLead->getValue(&readSize, xPT_startSpot, true);

	tPlug<cByteBuffer> readInto;
	read(&readInto.get(), readSize, readStart);
	aLead->setPlug(&readInto, xPT_buffer, false);
}

void
cBase_fileIO::patWrite(ptrLead aLead){

	tPlug<cByteBuffer>  writeHere;
	write(&writeHere.get());
	aLead->setPlug(&writeHere, xPT_buffer, false);
}

void
cBase_fileIO::patInsert(ptrLead aLead){
	size_t startSpot = 0;

	aLead->getValue(&startSpot, xPT_startSpot, true);
	tPlug<cByteBuffer> current = aLead->getPlug(xPT_buffer);
	insert( &current.get(), startSpot );
	aLead->setPlug(&current, xPT_buffer);
}

void
cBase_fileIO::patDelFile(ptrLead aLead){
	deleteFile();
}

void
cBase_fileIO::patGetFileSize(ptrLead aLead){
	PROFILE;
	mFileSize = getFileSize();
	aLead->setPlug(&mFileSize, xPT_fileSize);
}

