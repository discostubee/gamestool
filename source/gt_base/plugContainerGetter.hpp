/*
 * !\file	plugContainer.hpp
 * !\brief	If you need to serialize or pass about a container, you need a plug container.
 * !		Plug containers are plugs that use STL containers to hold other plugs. There's no
 * !		reason (that I can think of) to have a plug that uses an STL container for normal
 * !		data types.
 * !\note	Plug containers do NOT provide thread safety. That's up to the individual plugs they
 * !		contain.
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
*/

#ifndef PLUGCONTAINERGETTER_HPP
#define PLUGCONTAINERGETTER_HPP

#include "basePlug.hpp"
#include <vector>
#include <list>

///////////////////////////////////////////////////////////////////////////////////
// Objects.
namespace gt{

	//----------------------------------------------------------------------------------------------------------------
	//!\brief	Allows for easy specialisation.
	template<
		typename PLUG_T,
		template<typename, typename> class CONT_T
	>
	class tGetterPlug{
	public:
		typedef CONT_T<
			PLUG_T,
			std::allocator< PLUG_T >
		> dContainer;

		static
		cBase_plug*
		get(dContainer &from, size_t idx)
			{DONT_USE_THIS; return NULL;}

		static
		const cBase_plug*
		getConst(const dContainer &from, size_t idx)
			{DONT_USE_THIS; return NULL;}

	};


}

///////////////////////////////////////////////////////////////////////////////////
// specialisations.
namespace gt{

	template<typename PLUG_T>
	class tGetterPlug<PLUG_T, std::vector>{
	public:
		typedef std::vector<
			PLUG_T,
			std::allocator< PLUG_T >
		> dContainer;

		static
		cBase_plug*
		get(dContainer &from, size_t idx){
			ASRT_INRANGE(from, idx);
			return &from[idx];
		}

		static
		const cBase_plug*
		getConst(const dContainer &from, size_t idx){
			ASRT_INRANGE(from, idx);
			return &from[idx];
		}
	};

	template<typename PLUG_T>
	class tGetterPlug<PLUG_T, std::list>{
	public:
		typedef std::list<
			PLUG_T,
			std::allocator< PLUG_T >
		> dContainer;

		static
		cBase_plug*
		get(dContainer &from, size_t idx){
			ASRT_INRANGE(from, idx);
			typename dContainer::iterator itr( from.begin() );
			for(size_t i=0; i < idx; ++i)
				++itr;
			return &(*itr);
		}

		static
		const cBase_plug*
		getConst(const dContainer &from, size_t idx){
			ASRT_INRANGE(from, idx);
			typename dContainer::const_iterator itr( from.begin() );
			for(size_t i=0; i < idx; ++i)
				++itr;
			return &(*itr);
		}
	};





}


#endif
