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

///////////////////////////////////////////////////////////////////////////////////
namespace gt{

	//!\brief	Simple gamestool container for UTF-8 text.
	class cTextFig: public cFigment{
	public:
		static const cPlugTag* xPT_text;
		static const cCommand::dUID xSetText;
		static const cCommand::dUID xGetText;

		cTextFig();
		virtual ~cTextFig();

		GT_IDENTIFY("text");
		GT_EXTENDS(cFigment);
		GT_VERSION(1);
		virtual dNameHash hash() const{ return getHash<cTextFig>(); }

	protected:
		void patSetText(ptrLead aLead);
		void patGetText(ptrLead aLead);

	private:
		tPlug<dText> mText;
	};

}


///////////////////////////////////////////////////////////////////////////////////
//
namespace gt{

	template<>
	class cAnyOp::tOps<dText>{
	private:
		static void assignText(const dText & pFrom, void *pTo){
			reinterpret_cast<dText*>(pTo)->t.assign(pFrom);
		}

		static void appendText(const dText & pFrom, void *pTo){
			reinterpret_cast<dText*>(pTo)->t.append(pFrom);
		}

		static void assignNStr(const dText & pFrom, void *pTo){
			reinterpret_cast<dNatStr*>(pTo)->t.assign( toNStr(pFrom) );
		}

		static void appendNStr(const dText & pFrom, void *pTo){
			reinterpret_cast<dNatStr*>(pTo)->t.append( toNStr(pFrom) );
		}

		static void assignDStr(const dText & pFrom, void *pTo){
		}

		static void appendDStr(const dText & pFrom, void *pTo){
		}

	public:
		static void setup(tKat<dText> * pK, cAnyOp * pUsing){
			pK->addAss(&getRef(), genPlugType<dText>(), assignText);
			pK->addApp(&getRef(), genPlugType<dText>(), appendText);
			pK->addAss(&getRef(), genPlugType<dNatStr>(), assignNStr);
			pK->addApp(&getRef(), genPlugType<dNatStr>(), appendNStr);
			pK->addAss(&getRef(), genPlugType<dStr>(), assignDStr);
			pK->addApp(&getRef(), genPlugType<dStr>(), appendDStr);
		}
	};

	template<>
	class cAnyOp::tOps<dNatStr>{
	private:
		static void assignText(const dNatStr & pFrom, void *pTo){
		}

		static void appendText(const dNatStr & pFrom, void *pTo){
		}

		static void assignNStr(const dNatStr & pFrom, void *pTo){
		}

		static void appendNStr(const dNatStr & pFrom, void *pTo){
		}

		static void assignDStr(const dNatStr & pFrom, void *pTo){
		}

		static void appendDStr(const dNatStr & pFrom, void *pTo){
		}

	public:
		static void setup(tKat<dNatStr> * pK, cAnyOp * pUsing){
			pK->addAss(&getRef(), genPlugType<dText>(), assignText);
			pK->addApp(&getRef(), genPlugType<dText>(), appendText);
			pK->addAss(&getRef(), genPlugType<dNatStr>(), assignNStr);
			pK->addApp(&getRef(), genPlugType<dNatStr>(), appendNStr);
			pK->addAss(&getRef(), genPlugType<dStr>(), assignDStr);
			pK->addApp(&getRef(), genPlugType<dStr>(), appendDStr);
		}
	};

	template<>
	class cAnyOp::tOps<dStr>{
	private:
		static void assignText(const dStr & pFrom, void *pTo){
		}

		static void appendText(const dStr & pFrom, void *pTo){
		}

		static void assignNStr(const dStr & pFrom, void *pTo){
		}

		static void appendNStr(const dStr & pFrom, void *pTo){
		}

		static void assignDStr(const dStr & pFrom, void *pTo){
		}

		static void appendDStr(const dStr & pFrom, void *pTo){
		}

	public:
		static void setup(tKat<dStr> * pK, cAnyOp * pUsing){
			pK->addAss(&getRef(), genPlugType<dText>(), assignText);
			pK->addApp(&getRef(), genPlugType<dText>(), appendText);
			pK->addAss(&getRef(), genPlugType<dNatStr>(), assignNStr);
			pK->addApp(&getRef(), genPlugType<dNatStr>(), appendNStr);
			pK->addAss(&getRef(), genPlugType<dStr>(), assignDStr);
			pK->addApp(&getRef(), genPlugType<dStr>(), appendDStr);
		}
	};
}



#endif
