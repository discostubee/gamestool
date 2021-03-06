/*
 * !\file	base_fileIO.hpp
 * !\brief
 *
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

#ifndef BASE_FILEIO_HPP
#define BASE_FILEIO_HPP

#include "figment.hpp"

namespace gt{

	//--------------------------------------------------------
	//!\class	cBase_fileIO
	//!\brief	Interface for file IO, be it a local file or one
	//!			that's online.
	class cBase_fileIO: public cFigment{

	public:
		typedef unsigned int dFilePoint; //<!

		static const unsigned int ENDOF_FILE = static_cast<unsigned int>(-1);	//!< special identifier used mostly by insert function.

		static const cPlugTag*	xPT_buffer;
		static const cPlugTag*	xPT_startSpot;
		static const cPlugTag*	xPT_readSize;
		static const cPlugTag*	xPT_fileSize;
		static const cPlugTag*	xPT_filePath;

		static const cCommand::dUID	xSetPath;	//!< Set the path to the file you want to manipulate.
		static const cCommand::dUID	xRead;		//!< Write to the buffer from the file at the path. Reads the entire file if the xPT_readSize is 0 or not specified.
		static const cCommand::dUID	xWrite;		//!< Write the buffer to the file specified with set path. Creates the file if it doesn't exist. Re-writes the file if it does.
		static const cCommand::dUID	xInsert;	//!< Increases the file size as needed.
		static const cCommand::dUID	xDeleteFile;//!< Deletes the file if it exists, or does nothing if it doesn't.
		static const cCommand::dUID	xGetSize;	//!< Can be used to test if a file exists.

		cBase_fileIO();
		virtual ~cBase_fileIO();

		GT_IDENTIFY("file");
		GT_EXTENDS(cFigment);
		GT_VERSION(1);
		virtual dNameHash hash() const{ return getHash<cBase_fileIO>(); }

	protected:
		tPlug<dStr> mPath; //!< This is the path to the file.
		tPlug<size_t> mFileSize;

		void patSetPath(ptrLead aLead);
		void patRead(ptrLead aLead);
		void patWrite(ptrLead aLead);
		void patInsert(ptrLead aLead);
		void patDelFile(ptrLead aLead);
		void patGetFileSize(ptrLead aLead);

		//!\brief	Fills the provided buffer with file contents.
		//!\brief	aOutput
		//!\param	pStartPoint		0 means starts at the beginning of the file.
		//!\param	pReadAmount		0 means read the entire file.
		virtual void read(cByteBuffer* aOutput, const dFilePoint pStartPoint = 0, const size_t pReadAmount = 0 )
			{ DUMB_REF_PAR(pStartPoint); DUMB_REF_PAR(pReadAmount); DONT_USE_THIS; }

		virtual void write(const cByteBuffer* pBuff)
			{ DUMB_REF_PAR(pBuff); DONT_USE_THIS; }

		//!\brief	inserts data to some point in the file.
		//!\param	pBuff		Buffer to be sandwiched in the file.
		//!\param	pStartSpot	Spot to insert in the file. Use ENDOF_FILE if you want to append to, well, the end of the file.
		virtual void insert(const cByteBuffer* pBuff, dFilePoint pStartSpot)
			{ DUMB_REF_PAR(pBuff); DUMB_REF_PAR(pStartSpot); DONT_USE_THIS; }

		//!\brief	Delete the file.
		virtual void deleteFile()
			{ DONT_USE_THIS; }

		//!\brief	Get the size of a file.
		virtual size_t getFileSize()
			{ DONT_USE_THIS; return 0; }
	};
}

#endif
