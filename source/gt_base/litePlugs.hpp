/*
 * !\file	lead.hpp
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
 *
 */

#ifndef	LITEPLUGS_HPP
#define LITEPLUGS_HPP

#include "basePlug.hpp"

///////////////////////////////////////////////////////////////////////////////////
// Classes
namespace gt{

	//----------------------------------------------------------------------------------------------------------------
	//!\brief	Used just for copying and appending. References data rather than copies. Be careful that the reference
	//!			you pass is valid for the life of that reference.
	template<typename T>
	class tLitePlug: public tDataPlug<T>{
	public:
		tLitePlug(const tLitePlug &copyMe) : mRef( const_cast<T*>(copyMe.mRef) ) {}
		tLitePlug(T *aRef) : mRef(aRef) {}
		~tLitePlug() {}

		tLitePlug<T>& operator= (const tLitePlug<T> &aCopyMe) {
			ASRT_NOTSELF(&aCopyMe);
			mRef = aCopyMe.mRef;
			return *this;
		}

		T& get(){ return *mRef; }
		const T& getConst() const{ return *mRef; }

		//---
		void save(cByteBuffer* pSaveHere) { DONT_USE_THIS; }
		void loadEat(cByteBuffer* pChewToy, dReloadMap *aReloads = NULL) { DONT_USE_THIS; }

#		ifdef GT_THREADS
			void updateStart() { DONT_USE_THIS; }
			void updateFinish() { DONT_USE_THIS; }
#		endif

	protected:
#		ifdef GT_THREADS
			void readShadow(cBase_plug *pWriteTo, dConSig aCon) { DONT_USE_THIS; }
			void writeShadow(const cBase_plug *pReadFrom, dConSig aCon) { DONT_USE_THIS; }
			virtual void shadowAppends(cBase_plug *pWriteTo, dConSig pSig) { DONT_USE_THIS; }
			virtual void appendShadow(cBase_plug *pReadFrom, dConSig pSig) { DONT_USE_THIS; }
#		endif

	private:
		T *mRef;
	};

	//----------------------------------------------------------------------------------------------------------------
	//!\brief	Used just for copying and appending with constant.
	template<typename T>
	class tLitePlugConst: public tDataPlug<T>{
	public:
		tLitePlugConst(const T *aRef) : mRef(aRef) {}
		~tLitePlugConst(){}

		tLitePlug<T>& operator= (const tLitePlugConst<T> &aCopyMe) {
			ASRT_NOTSELF(&aCopyMe);
			mRef = aCopyMe.mRef;
			return *this;
		}

		T& get(){ DONT_USE_THIS; }
		const T& getConst() const{ return *mRef; }

		//---
		void save(cByteBuffer* pSaveHere) { DONT_USE_THIS; }
		void loadEat(cByteBuffer* pChewToy, dReloadMap *aReloads = NULL) { DONT_USE_THIS; }

		#ifdef GT_THREADS
			void updateStart(){ DONT_USE_THIS; }
			void updateFinish(){ DONT_USE_THIS; }
		#endif

	protected:
	#		ifdef GT_THREADS
			void readShadow(cBase_plug *pWriteTo, dConSig aCon){ DONT_USE_THIS; }
			void writeShadow(const cBase_plug *pReadFrom, dConSig aCon){ DONT_USE_THIS; }
			virtual void shadowAppends(cBase_plug *pWriteTo, dConSig pSig){ DONT_USE_THIS; }
			virtual void appendShadow(cBase_plug *pReadFrom, dConSig pSig){ DONT_USE_THIS; }
	#		endif

	private:
		const T *mRef;

		tLitePlugConst(T *aRef){}
	};

}





#endif
