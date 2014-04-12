/*
 * !\file	utils.hpp
 * !\brief	Contains all kinds of stand alone tools.
 */

#ifndef LINUX_FILEIO_HPP
#define LINUX_FILEIO_HPP

#include <fstream>
#include <gt_base/fileIO.hpp>
#include "linux_world.hpp"

namespace gt{

	class cLinux_fileIO: public cBase_fileIO{

	public:
		cLinux_fileIO();
		virtual ~cLinux_fileIO();

		GT_IDENTIFY("file linux");
		GT_EXTENDS(cBase_fileIO);
		GT_REPLACES(cBase_fileIO);
		virtual dNameHash hash() const{ return getHash<cLinux_fileIO>(); }

	protected:
		void read(cByteBuffer* aOutput, const dFilePoint pStartPoint = 0, const size_t pReadAmount = 0);
		void write(const cByteBuffer* pBuff);
		void insert(const cByteBuffer* pBuff, dFilePoint pStartPoint);
		void deleteFile();
		size_t getFileSize();
	};
}

#endif
