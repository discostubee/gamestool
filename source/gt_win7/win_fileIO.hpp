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
		static const dNatChar* identify(){ return "win file IO"; }
		static dNameHash replaces(){ return getHash<cBase_fileIO>(); }
		static void requirements();

		cWin_fileIO();
		virtual ~cWin_fileIO();

		//- standard
		virtual const dNatChar* name() const { return cWin_fileIO::identify(); }		//!<
		virtual dNameHash hash() const { return tOutline<cWin_fileIO>::hash(); }

	protected:
		//- polymorphs
		virtual cByteBuffer& read(const dFilePoint pStartPoint = 0, const size_t pReadAmount = 0 );
		virtual void write(const cByteBuffer* pBuff);
		virtual void insert(const cByteBuffer* pBuff, dFilePoint pStartPoint);
		virtual void deleteFile();
		virtual size_t getFileSize();
	};

}

#endif
