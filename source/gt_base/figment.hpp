/*
 * !\file	figment.hpp
 * !\brief	Contains both the base implementation of the figment, and the two other basic figment types: empty figment and the world ender.
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
#include "plug.hpp"

///////////////////////////////////////////////////////////////////////////////////
// classes
namespace gt{

	//-------------------------------------------------------------------------------------
	//!\brief	A figment of your imagination! More specifically, it's the base class type
	//!			for all the funky new stuff you'll make.
	class cFigment: public cFigContext, private tOutline<cFigment>{
	public:
		//-----------------------------
		// Commands and plug tags
		static const cPlugTag*	xPT_saveData;	//!<

		static const cCommand::dUID	xSave;	//!< Serialization is a base level ability.
		static const cCommand::dUID	xLoad;	//!< Ditto above.

		#if defined(DEBUG) && defined(GT_SPEED)
			static const cCommand::dUID xTestJack;
			static const cPlugTag* xPT_life;
		#endif

		//-----------------------------
		cFigment();
		virtual ~cFigment();

		//!\brief	Jack is your interface for using data with this figment. You shouldn't need to override this.
		virtual void jack(ptrLead pLead, cContext* pCon);

		//-----------------------------
		// These things are REQUIRED for any figment class.

		//!\brief	Used to define the string name of this object. It is also hashed to give the unique number
		//!			used to quickly compare objects.
		//!\note	You MUST replace this to identify your own
		static const char* identify(){ return "figment"; }

		//- Not sure how I could eliminate this. I should be able to.
		virtual const char* name() const { return identify(); }		//!< Virtual version of identify.
		virtual dNameHash hash() const { return tOutline<cFigment>::hash(); }

		//-----------------------------
		// standard interface. These are all optional in later classes.

		virtual void run(cContext* pCon);				//!< Gives the figment some runtime to do whatever it is that it normally does. Uses context to ensure it doesn't run into itself or other threads.

		//!\brief	If a non zero number is returned, this object replaces another in the world factory.
		//!			For instance, a base level file IO object needs to be replaced with a linux or windows
		//!			specific version.
		//!\note	You'll also need to specify the polymorphic function below as well.
		static dNameHash replaces(){ return uDoesntReplace; }
		virtual dNameHash getReplacement() const{ return replaces(); }	//!<	You'll need to override this if you are replacing stuff.

		//!\brief	If you want your figment to support the commands and tags from its parent, you'll need to extend from the parent.
		//!\note	You don't need to extend if you have replaced a parent.
		static dNameHash extends(){ return uDoesntExtend; }
		virtual dNameHash getExtension() const { return extends(); }	//!\<	You'll need to override this if you are replacing stuff.

		//- These are currently not threadsafe.
		virtual void save(cByteBuffer* pAddHere);		//!< Adds to the buffer, all the data needed to reload itself. It was done this way as opposed to a return auto pointer because all save operations are buffer appends.
		virtual void loadEat(cByteBuffer* pBuff, dReloadMap *aReloads = NULL);			//!< Called load-eat because the head of the buffer is consume by the load function.
		virtual void getLinks(std::list<ptrFig>* pOutLinks);	//!< Append the list being passed in, with any figment pointers which form the run structure of the program.

	protected:

		//-----------------------------
		// Patch through functions for use with command.
		void patSave(ptrLead aLead);	//!< Allows you to call the save function using jack
		void patLoad(ptrLead aLead);	//!< Same asa the patSave function above.

		#if defined(DEBUG) && defined(GT_SPEED)
			void patTestJack(ptrLead aLead);
		#endif
	};

	//-------------------------------------------------------------------------------------
	//!\brief	Designed to be the null return type. It doesn't do anything and it allows
	//!			objects to still function by referencing this dummy instead of null.
	//!\note	This is an example of the bare minimum that you have to do in order to make
	//!			a new figment type class.
	class cEmptyFig: public cFigment, private tOutline<cEmptyFig>{
	public:
		static const char* identify(){ return "empty figment"; }

		cEmptyFig();
		virtual ~cEmptyFig();

		virtual const char* name() const{ return cEmptyFig::identify(); }
		virtual dNameHash hash() const{ return tOutline<cEmptyFig>::hash(); }
	};

	//-------------------------------------------------------------------------------------
	//!\brief	Use this object to cause the program to no longer loop, and so exit. However,
	//!			this is not an instant shutdown because the program must still finish the loop
	//!			it is on. Simply run it to shutoff the loop.
	//!\note	Should be called Unicron.
	class cWorldShutoff: public cFigment, private tOutline<cWorldShutoff>{
	public:
		static const char* identify(){ return "world shutoff"; }

		cWorldShutoff();
		virtual ~cWorldShutoff();

		virtual const char* name() const{ return identify(); }
		virtual dNameHash hash() const{ return tOutline<cWorldShutoff>::hash(); }

		virtual void run(cContext* pCon);
	};
}

///////////////////////////////////////////////////////////////////////////////////
// template specializations
namespace gt{

	//--------------------------------------
	template<>
	class tPlug<ptrFig>: public tPlugShadows<ptrFig>{
	public:
		ptrFig mD;

		tPlug() : tPlugShadows<ptrFig>(typeid(ptrFig)), mD(gWorld.get()->getEmptyFig()){
		}

		tPlug(ptrFig pA) : tPlugShadows<ptrFig>(typeid(ptrFig)), mD(pA){
		}

		tPlug(const tPlug<ptrFig> &other) : tPlugShadows<ptrFig>(typeid(ptrFig)), mD(other.mD){
		}

		virtual ~tPlug(){}

		virtual cBase_plug& operator= (const cBase_plug &pD){
			NOTSELF(&pD);
			PROFILE;

			//!\todo figure out a way to prevent code duplication.
			if( mType == pD.mType ){	// we can just cast
				//cBase_plug* temp = const_cast<cBase_plug*>(&pD);
				mD = dynamic_cast< tPlug<ptrFig>* >(
					const_cast<cBase_plug*>(&pD)
				)->mD;
			}else{
				PLUG_CANT_COPY_ID(mType, pD.mType);
			}

			return *this;
		}

		virtual cBase_plug& operator= (const cBase_plug* pD){
			NOTSELF(pD);
			PROFILE;

			//!\todo figure out a way to prevent code duplication.
			if( mType == pD->mType ){	// we can just cast
				//cBase_plug* temp = const_cast<cBase_plug*>(&pD);
				mD = dynamic_cast< tPlug<ptrFig>* >(
					const_cast<cBase_plug*>(pD)
				)->mD;
			}else{
				PLUG_CANT_COPY_ID(mType, pD->mType);
			}

			return *this;
		}

		cBase_plug& operator= (ptrFig pA){ mD = pA; return *this; }

		cBase_plug& operator= (const tPlug<ptrFig> &other){ if(this != &other) mD = other.mD; return *this; }

		virtual void save(cByteBuffer* pAddHere){
			PROFILE;

			//- Using the pointer as a unique number to identify the referenced figment.
			dFigSaveSig saveSig = reinterpret_cast<dFigSaveSig>( mD.get() );
			pAddHere->add( (dByte*)(&saveSig), sizeof(dFigSaveSig) );

			//DBUG_LO("	Saved as" << reinterpret_cast<unsigned long>(saveSig));
		}

		virtual void loadEat(cByteBuffer* pChewToy, dReloadMap *aReloads){
			PROFILE;

			dFigSaveSig saveSig = 0;
			pChewToy->fill(&saveSig);
			pChewToy->trimHead(sizeof saveSig);

			dReloadMap::iterator itr = aReloads->find(saveSig);

			if(itr == aReloads->end())
				throw excep::notFound("signature of reloaded figment", __FILE__, __LINE__);	//- figment remains empty.

			mD = itr->second->fig;
		}

		virtual void reset(cContext* context){
			DUMB_REF_ARG(context);
			mD = gWorld.get()->getEmptyFig();
		}

	protected:
		#ifdef GT_THREADS
			virtual ptrFig& getMD() { return mD; }
		#endif
	};

}

#endif
