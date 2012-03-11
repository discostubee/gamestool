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

#ifndef TEXTFIG_HPP
#define TEXTFIG_HPP

#include "figment.hpp"

namespace gt{

	//!\note	Stores string types in a format that is most efficient to this compilation of gamestool.
	//!			When saving, it is converted into a string format that is consistent across all compilations.
	class cTextFig: public cFigment, private tOutline<cTextFig>{
	public:
		static const cPlugTag* xPT_text;
		static const cCommand::dUID xSetText;
		static const cCommand::dUID xGetText;

		cTextFig();
		virtual ~cTextFig();

		static const dNatChar* identify(){ return "text fig"; }
		virtual const dNatChar* name() const{ return cTextFig::identify(); }
		virtual dNameHash hash() const{ return tOutline<cTextFig>::hash(); }

		virtual void run(cContext* pCon);				//!< Temp code to display text in console.
		virtual void save(cByteBuffer* pAddHere);
		virtual void loadEat(cByteBuffer* pBuff, dReloadMap* pReloads);

	protected:
		void patSetText(ptrLead aLead);
		void patGetText(ptrLead aLead);

	private:
		tPlug<std::string> mText;
	};
}


///////////////////////////////////////////////////////////////////////////////////
// Template specializations.
namespace gt{

	//------------------------------------------------------------------------------------------------------------------
	//!\brief	Your program will store strings in whatever format it was built with. However, it will have to save and
	//!			load using UTF16.
	template<>
	class tPlug<dText>: public tPlugShadows<dText>{
	public:
		dText mD;

		tPlug(): tPlugShadows<dText>(typeid(dStr)){
		}

		tPlug(dText pA): tPlugShadows<dText>(typeid(dStr)), mD(pA){
		}

		virtual ~tPlug(){}

		virtual cBase_plug& operator= (const cBase_plug &pD){
			NOTSELF(&pD);

			pD.copyInto(&mD);

			return *this;
		}

		virtual cBase_plug& operator= (const cBase_plug* pD){
			NOTSELF(pD);

			pD->copyInto(&mD);

			return *this;
		}

		virtual bool operator == (const cBase_plug &pD) const{
			if( tPlugShadows<dText>::mType == pD.mType ){	// we can just cast
				return mD.compare( const_cast<cBase_plug*>(&pD)->exposePtr<dText>()->c_str() ) == 0;
			}

			throw excep::base_error("Can't compare", __FILE__, __LINE__);
		}

		void operator= (const dNatChar &pA){
			//!\todo	conversion.
		}

		void save(cByteBuffer* pAddHere){
			//!\todo Save as UTF16.

			//!\todo	Avoid temporary buffer.
			const size_t length = mD.size();
			const size_t totalSize = (length * sizeof(dTextChar))+sizeof(size_t);	// each character should only be 1 byte in size.
			dByte* temp = new dByte[totalSize];

			try{
				::memcpy( temp, &length, sizeof(size_t) );
				for(size_t idx=0; idx < length; ++idx){
					//!\todo make a more sophisticated conversion.
					temp[idx+sizeof(size_t)] = mD.at(idx);
				}

				pAddHere->add(temp, totalSize);
			}catch(...){
				delete [] temp; throw;
			}
			delete [] temp;
		}

		void loadEat(cByteBuffer* pBuff, dReloadMap *aReloads){
			//!\todo Load as UTF16.

			DUMB_REF_ARG(aReloads);

			if(pBuff->size() < sizeof(size_t))
				throw cByteBuffer::excepUnderFlow(__FILE__, __LINE__);

			size_t length = *(reinterpret_cast<const size_t*>(pBuff->get()));	// Length in 8 bit ascii.

			if(length==0)
				return;

			if(pBuff->size() < sizeof(size_t) + length )
				throw cByteBuffer::excepUnderFlow(__FILE__, __LINE__);

			mD.clear();
			mD.reserve(length);

			for(size_t idx=0; idx< length; ++idx){
				//!\todo make more sophisticated conversion.
				mD.push_back( *pBuff->get(sizeof(size_t)+(idx * sizeof(dTextChar)) ) );
			}

			pBuff->trimHead(sizeof(size_t)+length);
		}

		void reset(){
			mD.clear();
		}

	protected:
		virtual dText& getMD() {
			return mD;
		}
	};
}


#endif
