/*
 * !\file	plugContainerOps.hpp
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

#ifndef PLUGCONTAINEROPS_HPP
#define PLUGCONTAINEROPS_HPP

#include "plugContainer.hpp"

///////////////////////////////////////////////////////////////////////////////////
// Objects.
namespace gt{

	//----------------------------------------------------------------------------------------------------------------
	//!\brief
	template<
		typename ELEM_T,
		template<typename, typename> class CONT_T
	>
	class tCommonContainerOps{
	public:
		typedef typename tPlugLinearContainer<ELEM_T, CONT_T>::dContainer dMyType;

		static void ass(const dMyType & pFrom, void *pTo){
			reinterpret_cast< dMyType* >(pTo)->assign(pFrom.begin(), pFrom.end());
		}

		static void app(const dMyType & pFrom, void *pTo){
			reinterpret_cast< dMyType* >(pTo)->assign(pFrom.begin(), pFrom.end());
		}

		static void peek(const dMyType & pFrom, void *pTo){
			if(pFrom.empty())
				throw excep::underFlow(__FILE__, __LINE__);

			*reinterpret_cast<ELEM_T*>(pTo) = pFrom.begin()->getConst();
		}

		static void appSingleElem(const ELEM_T & pFrom, void *pTo){
			reinterpret_cast<dMyType*>(pTo)->push_back(
				tPlug<ELEM_T>(pFrom)
			);
		}

		static void appSinglePlug(const tPlug<ELEM_T> & pFrom, void *pTo){
			reinterpret_cast<dMyType*>(pTo)->push_back(pFrom);
		}

		template< template<typename, typename> class OTHER_T >
		class tOtherContainer{
		public:
			typedef typename tPlugLinearContainer<ELEM_T, OTHER_T>::dContainer dOtherType;

			static void ass(const dMyType & pFrom, void *pTo){
				reinterpret_cast< dOtherType* >(pTo)->assign(pFrom.begin(), pFrom.end());
			}

			static void app(const dMyType & pFrom, void *pTo){
				dOtherType * ref =reinterpret_cast< dOtherType* >(pTo);
				ref->insert(ref->end(), pFrom.begin(), pFrom.end());
			}
		};
	};

}


///////////////////////////////////////////////////////////////////////////////////
// Template specialisations.
namespace gt{


	template< typename ELEM_T >
	class cAnyOp::tOps< std::vector< tPlug<ELEM_T> > >{
	public:
		static void setup(
			tKat< typename tPlugLinearContainer<ELEM_T, std::vector>::dContainer > * pK,
			cAnyOp * pUsing
		){
			typedef tCommonContainerOps<ELEM_T, std::vector> myOps;
			typedef typename tCommonContainerOps<ELEM_T, std::vector>
				::template tOtherContainer<std::list> listOps;

			pK->addAss(&getRef(), genPlugType< typename myOps::dMyType >(), myOps::ass);
			pK->addApp(&getRef(), genPlugType< typename myOps::dMyType >(), myOps::app);
			pK->addAss(&getRef(), genPlugType< typename listOps::dOtherType >(), listOps::ass);
			pK->addApp(&getRef(), genPlugType< typename listOps::dOtherType >(), listOps::app);
			pK->addAss(&getRef(), genPlugType<ELEM_T>(), myOps::peek);

			cAnyOp::tKat<ELEM_T>::xKat.addApp(
				&getRef(),
				genPlugType< typename myOps::dMyType >(),
				myOps::appSingleElem
			);
			cAnyOp::tKat< tPlug<ELEM_T> >::xKat.addApp(
				&getRef(),
				genPlugType< typename myOps::dMyType >(),
				myOps::appSinglePlug
			);
		}
	};

	template< typename ELEM_T >
	class cAnyOp::tOps< std::list< tPlug<ELEM_T> > >{
	public:
		static void setup(
			tKat< typename tPlugLinearContainer<ELEM_T, std::list>::dContainer > * pK,
			cAnyOp * pUsing
		){
			typedef tCommonContainerOps<ELEM_T, std::list> myOps;
			typedef typename tCommonContainerOps<ELEM_T, std::list>
				::template tOtherContainer<std::vector> vecOps;

			pK->addAss(&getRef(), genPlugType< typename myOps::dMyType >(), myOps::ass);
			pK->addApp(&getRef(), genPlugType< typename myOps::dMyType >(), myOps::app);
			pK->addAss(&getRef(), genPlugType< typename vecOps::dOtherType >(), vecOps::ass);
			pK->addApp(&getRef(), genPlugType< typename vecOps::dOtherType >(), vecOps::app);
			pK->addAss(&getRef(), genPlugType<ELEM_T>(), myOps::peek);

			cAnyOp::tKat<ELEM_T>::xKat.addApp(
				&getRef(),
				genPlugType< typename myOps::dMyType >(),
				myOps::appSingleElem
			);
			cAnyOp::tKat< tPlug<ELEM_T> >::xKat.addApp(
				&getRef(),
				genPlugType< typename myOps::dMyType >(),
				myOps::appSinglePlug
			);
		}
	};
}

#endif
