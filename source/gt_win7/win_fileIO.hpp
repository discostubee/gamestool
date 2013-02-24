/*
 * !\file	utils.hpp
 * !\brief	Contains all kinds of stand alone tools.
 */

#ifndef WIN_FILEIO_HPP
#define WIN_FILEIO_HPP

#include <fstream>
#include <gt_base/fileIO.hpp>
#include "win_world.hpp"

namespace gt{

	class cWin_fileIO: public cBase_fileIO, private tOutline<cWin_fileIO>{
	public:
		cWin_fileIO();
		virtual ~cWin_fileIO();

		GT_IDENTIFY("win fileIO");
		GT_EXTENDS(cFigment);
		virtual dNameHash hash() const { return getHash<cWin_fileIO>(); }

	protected:
		virtual void read(cByteBuffer* aOutput, const dFilePoint pStartPoint = 0, const size_t pReadAmount = 0 );
		virtual void write(const cByteBuffer* pBuff);
		virtual void insert(const cByteBuffer* pBuff, dFilePoint pStartSpot);
		virtual void deleteFile();
		virtual size_t getFileSize();
	};

}

#endif
