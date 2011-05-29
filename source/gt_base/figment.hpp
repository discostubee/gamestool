/*
 * !\file	figment.hpp
 * !\brief
 *
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
 *
 */

#ifndef FIGMENT_HPP
#define FIGMENT_HPP

#include "blueprint.hpp"
#include "context.hpp"

///////////////////////////////////////////////////////////////////////////////////
// classes
namespace gt{

	//-------------------------------------------------------------------------------------
	//!\brief	A figment of your imagination! More specifically, it's the base class type
	//!			for all the funky new stuff you'll make.
	class cFigment: public iFigment, private tOutline<cFigment>{
	public:
		//-----------------------------
		// Defines
		static const cPlugTag*	xPT_buffer;
		static const cCommand*	xSave;	//!< \note ID=0
		static const cCommand*	xLoad;	//!< \note ID=1

		enum{
			eNotMyBag = uNotMyBag,
			eSave,
			eLoad,
			eSwitchEnd,
		};

		//-----------------------------
		// Statics members.

		//!\fn		static const dNatChar* identify() const
		//!\brief	Used to define the string name of this object. It is also hashed to give the unique number
		//!			used to quickly compare objects.
		//!\note	You MUST replace this to identify your own
		static const dNatChar* identify(){ return "figment"; }

		//!\fn		static dNameHash replaces() const
		//!\brief	If a non zero number is returned, this object replaces another in the world factory.
		//!			For instance, a base level file IO object needs to be replaced with a linux or windows
		//!			specific version.
		static dNameHash replaces(){ return uDoesntReplace; }

		//!\fn		static void requirements()
		//!\brief	This is used to make sure that all the figments this one requires, are drafted as well.
		static void requirements();

		//-----------------------------
		cFigment();
		virtual ~cFigment();

		//-----------------------------
		// constant polymorphs. These are not optional.
		virtual const dNatChar* name() const { return identify(); }		//!< Virtual version of identify.
		virtual dNameHash hash() const { return tOutline<cFigment>::hash(); }

		//-----------------------------
		// standard interface polymorphs. These are all optional in later classes.
		virtual void jack(ptrLead pLead);						//!< Jack is the interface used to get and set various members in a super generic fashion.
		virtual void run(cContext* pCon);						//!< Gives the figment some runtime to do whatever it is that it normally does. Gets passed a reference to cContext so that it can see what important figments were run befor it.
		virtual cByteBuffer& save();							//!< Outputs all the data it needs to re-load later into a byte buffer
		virtual void loadEat(cByteBuffer* pBuff, dReloadMap* pReloads = NULL){}				//!< Called load eat because the head of the buffer is consume by the load function.
		virtual void getLinks(std::list<ptrFig>* pOutLinks){}	//!< Append the list being passed in, with any figmentt pointers which form part of the program structure (which should be all of them).
		virtual dNameHash getReplacement() const{ return cFigment::replaces(); }	//!< You'll need to override this if you are replacing stuff.
	};

	//-------------------------------------------------------------------------------------
	//!\brief	Designed to be the null return type. It doesn't do anything and it allows
	//			objects to still function by referencing this dummy instead of null.
	//!\note	This is an example of the bare minimum that you have to do in order to make
	//!			a new figment type class.
	class cEmptyFig: public cFigment, private tOutline<cEmptyFig>{
	public:
		static const dNatChar* identify(){ return "empty figment"; }

		cEmptyFig();
		virtual ~cEmptyFig();

		virtual const dNatChar* name() const{ return cEmptyFig::identify(); }
		virtual dNameHash hash() const{ return tOutline<cEmptyFig>::hash(); }
	};

	//-------------------------------------------------------------------------------------
	//!\brief	Use this object to cause the program to no longer loop, and so exit. However,
	//!			this is not an instant shutdown because the program must still finish the loop
	//!			it is on. Simply run it to shutoff the loop.
	class cWorldShutoff: public cFigment, private tOutline<cWorldShutoff>{
	public:
		static const dNatChar* identify(){ return "world shutoff"; }

		cWorldShutoff();
		virtual ~cWorldShutoff();

		virtual const dNatChar* name() const{ return identify(); }
		virtual dNameHash hash() const{ return tOutline<cWorldShutoff>::hash(); }

		virtual void run(cContext* pCon);
	};
}

///////////////////////////////////////////////////////////////////////////////////
// template specializations
namespace gt{
	//--------------------------------------
	template<>
	class cPlug<ptrFig>: public cBase_plug{
	public:
		ptrFig mD;

		cPlug() : cBase_plug(typeid(ptrFig)), mD(gWorld->getEmptyFig()){
		}

		cPlug(ptrFig pA) : cBase_plug(typeid(ptrFig)), mD(pA){
		}

		virtual ~cPlug(){}

		virtual cBase_plug& operator= (const cBase_plug &pD){
			PROFILE;

			//!\todo figure out a way to prevent code duplication.
			if( mType == pD.mType ){	// we can just cast
				//cBase_plug* temp = const_cast<cBase_plug*>(&pD);
				mD = dynamic_cast< cPlug<ptrFig>* >(
					const_cast<cBase_plug*>(&pD)
				)->mD;
			}else{
				PLUG_CANT_COPY_ID(pD.mType);
			}

			return *this;
		}

		virtual void operator= (ptrFig pA){ mD = pA; }

		virtual cByteBuffer& save(){
			PROFILE;

			//- Using the pointer as a unique number to identify the referenced figment.
			cByteBuffer* saveBuff = new cByteBuffer();
			dFigSaveSig saveSig = static_cast<dFigSaveSig>( mD.get() );
			saveBuff->add( (dByte*)(&saveSig), sizeof(dFigSaveSig) );
			return *saveBuff;
		}

		virtual void loadEat(cByteBuffer* pChewToy, dReloadMap* pReloads){
			PROFILE;

			if(pReloads != NULL){
				dFigSaveSig saveSig = 0;
				pChewToy->fill(&saveSig);
				dReloadMap::iterator itr = pReloads->find(saveSig);

				mD = itr->second->fig;
				pChewToy->trim(sizeof saveSig);
			}
		}

		virtual void reset(){
			mD = gWorld->getEmptyFig();
		}
	};


}

#endif
