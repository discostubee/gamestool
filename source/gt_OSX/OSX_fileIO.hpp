/*
 * !\file	OSX_fileIO.hpp
 * !\brief
 *
 *  Copyright (C) 2011  Stuart Bridgens
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
 *
 */

#ifndef OSX_FILEIO_HPP
#define OSX_FILEIO_HPP

#include "OSX_world.hpp"
#include "gt_base/fileIO.hpp"

#include <fstream>

namespace gt{

	class cOSX_fileIO: public cBase_fileIO{
	public:

		cOSX_fileIO();
		virtual ~cOSX_fileIO();

		GT_IDENTIFY("OSX file");
		GT_EXTENDS(cBase_fileIO);
		virtual dNameHash hash() const{ return getHash<cOSX_fileIO>(); }

	protected:
		virtual cByteBuffer& read(const dFilePoint pStartPoint = 0, const size_t pReadAmount = 0 );
		virtual void write(const cByteBuffer* pBuff);
		virtual void insert(const cByteBuffer* pBuff, dFilePoint pStartPoint);
		virtual void deleteFile();
		virtual size_t getFileSize();
	};
}

#endif
