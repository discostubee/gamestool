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

	class cLinux_fileIO: public cBase_fileIO, private tOutline<cLinux_fileIO>{
	protected:
		virtual cByteBuffer& read(const dFilePoint pStartPoint = 0, const size_t pReadAmount = 0 );
		virtual void write(const cByteBuffer* pBuff);
		virtual void insert(const cByteBuffer* pBuff, dFilePoint pStartPoint);
		virtual void deleteFile();
		virtual size_t getFileSize();

	public:
		static const char* identify(){ return "linux file IO"; }
		static dNameHash replaces(){ return getHash<cBase_fileIO>(); }

		cLinux_fileIO();
		virtual ~cLinux_fileIO();

		virtual const char* name() const{ return cLinux_fileIO::identify(); }	//!<
		virtual dNameHash hash() const{ return getHash<cLinux_fileIO>(); }
	};
}

#endif
