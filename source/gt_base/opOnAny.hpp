/*
 * !\file	opOnAny.hpp
 * !\brief	This contains a bunch of implementations for ordinary data type for the opOnAny class.
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

#ifndef	OPONANY_HPP
#define OPONANY_HPP

#include "basePlug.hpp"
#include "binPacker.hpp"
#include <boost/shared_ptr.hpp>
#include <vector>
#include <map>

///////////////////////////////////////////////////////////////////////////////////
namespace gt{
	typedef boost::shared_ptr<cByteBuffer> ptrBuff;
}

///////////////////////////////////////////////////////////////////////////////////
namespace gt{

	//----------------------------------------------------------------------------------------------------------------
	namespace voidAssign{
		void textToNStr(const dText *pFrom, void *pTo);
		void textToPStr(const dText *pFrom, void *pTo);
		void plaCStrToPStr(const dPlaChar * const *pFrom, void *pTo);
		void plaCStrToNStr(const dPlaChar * const *pFrom, void *pTo);
		void plaCStrToText(const dPlaChar * const *pFrom, void *pTo);
	}

	//----------------------------------------------------------------------------------------------------------------
	//!\brief
	namespace voidAppend{
		void textToText(const dText *pFrom, void *pTo);
		void plaCStrToPStr(const dPlaChar * const *pFrom, void *pTo);
		void plaCStrToNStr(const dPlaChar * const *pFrom, void *pTo);
		void plaCStrToText(const dPlaChar * const *pFrom, void *pTo);
	}
}

///////////////////////////////////////////////////////////////////////////////////
// Template specializations
namespace gt{


	//-------------------------------------------------------------------------------------
	template<>
	class tOpOnAny< const dPlaChar* >{
	public:
		static tDataPlug< const dPlaChar* >::dMapAssigns * assign(){
			static bool setup = false;
			static tDataPlug<const dPlaChar*>::dMapAssigns copiers;

			if(!setup){
				copiers[ cBase_plug::genPlugType<dText>() ] = voidAssign::plaCStrToText;
				copiers[ cBase_plug::genPlugType<dNatStr>() ] = voidAssign::plaCStrToNStr;
				copiers[ cBase_plug::genPlugType<dStr>() ] = voidAssign::plaCStrToPStr;
				setup=true;
			}

			return &copiers;
		}

		static tDataPlug< const dPlaChar* >::dMapAssigns * append(){
			static bool setup = false;
			static tDataPlug< const dPlaChar* >::dMapAppends app;

			if(!setup){
				app[ cBase_plug::genPlugType<dText>() ] = voidAppend::plaCStrToText;
				app[ cBase_plug::genPlugType<dNatStr>() ] = voidAppend::plaCStrToNStr;
				app[ cBase_plug::genPlugType<dStr>() ] = voidAppend::plaCStrToPStr;
				setup=true;
			}

			return &app;
		}
	};

	//-------------------------------------------------------------------------------------
	template<>
	class tOpOnAny<dText>{
	public:
		static tDataPlug<dText>::dMapAssigns * assign(){
			static bool setup = false;
			static tDataPlug<dText>::dMapAssigns copiers;

			if(!setup){
				copiers[ cBase_plug::genPlugType<dText>() ] = voidAssign::basic<dText>;
				copiers[ cBase_plug::genPlugType<dNatStr>() ] = voidAssign::textToNStr;
				copiers[ cBase_plug::genPlugType<dStr>() ] = voidAssign::textToPStr;
				setup=true;
			}

			return &copiers;
		}

		static tDataPlug<dText>::dMapAssigns * append(){
			static bool setup = false;
			static tDataPlug<dText>::dMapAppends app;

			if(!setup){
				app[ cBase_plug::genPlugType<dText>() ] = voidAppend::textToText;
				setup=true;
			}

			return &app;
		}
	};

	//-------------------------------------------------------------------------------------
	template<>
	class tOpOnAny< ptrBuff >{
	public:

		static tDataPlug< ptrBuff >::dMapAssigns * assign(){
			static bool setup = false;
			static tDataPlug< boost::shared_ptr<cByteBuffer> >::dMapAssigns ass;

			if(!setup){
				setup=true;
			}

			return &ass;
		}

		static tDataPlug< ptrBuff >::dMapAppends * append(){
			static bool setup = false;
			static tDataPlug< boost::shared_ptr<cByteBuffer> >::dMapAppends app;

			if(!setup){
				setup=true;
			}

			return &app;
		}
	};
}

#endif
