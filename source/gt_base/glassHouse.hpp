/*
 * !\file	figment.hpp
 * !\brief	Contains both the base implementation of the figment, and the two other basic figment types: empty figment and the world ender.
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

#ifndef GLASSHOUSE_H
#define GLASSHOUSE_H

//!\brief	Stamps out all the things needed to make a pot inside your house.
#define STAMP_POT(HOUSE_TYPE, POT_TYPE, POT) \
	POT_TYPE POT;\
	static void setPot##POT(HOUSE_TYPE *house, const cBase_plug *set){ set->assign(&house->POT, cBase_plug::genPlugType<HOUSE_TYPE>()); }\
	static void getPot##POT(const HOUSE_TYPE *house, cBase_plug *get){ *get = house->POT; }


#include "figment.hpp"

namespace gt {

//!\brief	The glass house is about a structure that has multiple members, that you can access individually by a query.
//!			It's said structure because this only works on objects whose members are public, as in visible, as in anything
//!			inside a glass house (we'll call these pots) is public.
template<typename HOUSE>
class tGlassHouse: public tPlug<HOUSE>, public HOUSE{
public:
	typedef void (*fuSet)(HOUSE *to, const cBase_plug *from);
	typedef void (*fuGet)(const HOUSE *from, cBase_plug *to);

	class cPot{
	public:
		dNatStr mName;
		fuSet mSet;
		fuGet mGet;

		cPot(const dPlaChar *pName, fuSet pSet, fuGet pGet) : mName(toNStr(pName)), mSet(pSet), mGet(pGet) {}
		cPot(const dStr &pName, fuSet pSet, fuGet pGet) : mName(toNStr(pName)), mSet(pSet), mGet(pGet) {}
		cPot() : mSet(NULL), mGet(NULL) {}
	};

	typedef typename std::map<dNameHash, cPot> dName2Pot;

	//!\brief	Takes a series of pots (which are deleted when used) to create the pot map. Don't forget to null terminate.
	static dName2Pot eatPotsToMake(cPot *pPot, ...){
		dName2Pot pots;
		va_list params;

		va_start(params, pPot);
		while(pPot != NULL){
			pots.insert( std::pair<dNameHash, cPot>(
				makeHash(pPot->mName), cPot(pPot->mName, pPot->mSet, pPot->mGet)
			) );
			delete pPot;
			pPot = va_arg( params, cPot* );
		}

		return pots;
	}

	tGlassHouse();
	virtual ~tGlassHouse();

	void set(const cBase_plug *from, dNameHash name);	//!< Set the member with what's in the plug.
	void get(cBase_plug *to, dNameHash name);	//!< Get the member and put it in the plug.

private:
	static dName2Pot xPots;
};

/*
//!\brief	Gets a member from a glass house.
class cExtractor: public cFigment{
public:

	cExtractor();
	virtual ~cExtractor();
};
*/

}

///////////////////////////////////////////////////////////////////////////////////
// template definition
namespace gt{
	template<typename HOUSE>
	tGlassHouse<HOUSE>::tGlassHouse() {}

	template<typename HOUSE>
	tGlassHouse<HOUSE>::~tGlassHouse(){}

	template<typename HOUSE>
	void
	tGlassHouse<HOUSE>::set(const cBase_plug *from, dNameHash name) {
		xPots[name].mSet(this, from);
	}

	template<typename HOUSE>
	void
	tGlassHouse<HOUSE>::get(cBase_plug *to, dNameHash name){
		xPots[name].mGet(this, to);
	}

}

#endif
