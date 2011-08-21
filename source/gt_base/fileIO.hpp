/*
 * !\file	base_fileIO.hpp
 * !\brief
 */

#ifndef BASE_FILEIO_HPP
#define BASE_FILEIO_HPP

#include "figment.hpp"

namespace gt{

	//--------------------------------------------------------
	typedef unsigned int dFilePoint; //<!

	//--------------------------------------------------------
	//!\class	cBase_fileIO
	//!\brief	Interface for file IO, be it a local file or one
	//!			that's online.
	class cBase_fileIO: public cFigment, private tOutline<cBase_fileIO>{


	public:
		enum{
			eSetPath = cFigment::eSwitchEnd +1,
			eRead,
			eWrite,
			eInsert,
			eDeleteFile,
			eGetSize,
			eSwitchEnd
		};

		static const unsigned int ENDOF_FILE = static_cast<unsigned int>(-1);	//!< special identifier used mostly by insert function.
		static const cPlugTag*	xPT_buffer;
		static const cPlugTag*	xPT_startSpot;
		static const cPlugTag*	xPT_readSize;
		static const cPlugTag*	xPT_fileSize;
		static const cPlugTag*	xPT_filePath;

		static const cCommand*	xSetPath;	//!< set the path to the file you want to manipulate.
		static const cCommand*	xRead;		//!< write to the file specified with set path.
		static const cCommand*	xWrite;
		static const cCommand*	xInsert;
		static const cCommand*	xDeleteFile;
		static const cCommand*	xGetSize;

		cBase_fileIO();
		virtual ~cBase_fileIO();

		static const dNatChar* identify(){ return "base file IO"; }
		virtual const dNatChar* name() const { return identify(); }		//!<
		virtual dNameHash hash() const{ return tOutline<cBase_fileIO>::hash(); }

		virtual void jack(ptrLead pLead, cContext* pCon);
		virtual void run(cContext* pCon){};

	protected:
		tPlug<dStr> mPath; //!< This is the path to the file.
		tPlug<size_t> mFileSize;

		//!\fn		virtual cByteBuffer read(const dFilePoint pStartPoint = 0, const size_t pReadAmount = 0 )
		//!\brief	Returns a buffer containing what was loaded from a file.
		//!\param	pStartPoint		0 means starts at the beginning of the file.
		//!\param	pReadAmount		0 means read the entire file.
		virtual cByteBuffer& read(const dFilePoint pStartPoint = 0, const size_t pReadAmount = 0 )
			{ DUMB_REF_ARG(pStartPoint); DUMB_REF_ARG(pReadAmount); DONT_USE_THIS;  /*cByteBuffer *temp=NULL; return *temp;*/ }

		virtual void write(const cByteBuffer* pBuff)
			{ DUMB_REF_ARG(pBuff); DONT_USE_THIS; }

		//!\fn		virtual void append(const cByteBuffer& pBuff, const dFilePoint pStartPoint)
		//!\brief	inserts data to some point in the file.
		//!\param	pBuff		Buffer to be sandwiched in the file.
		//!\param	pStartSpot	Spot to insert in the file. Use ENDOF_FILE if you want to append to, well, the end of the file.
		virtual void insert(const cByteBuffer* pBuff, dFilePoint pStartSpot)
			{ DUMB_REF_ARG(pBuff); DUMB_REF_ARG(pStartSpot); DONT_USE_THIS; }

		//!\fn		virtual void deleteFile()
		//!\brief	Delete the file.
		virtual void deleteFile()
			{ DONT_USE_THIS; }

		//!\fn		virtual size_t getFileSize()
		//!\brief	Get the size of a file.
		virtual size_t getFileSize()
			{ DONT_USE_THIS; return 0; }
	};
}

#endif
