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

#ifndef FIGMENT_HPP
#define FIGMENT_HPP

#include "blueprint.hpp"
#include "context.hpp"
#include "plug.hpp"

///////////////////////////////////////////////////////////////////////////////////
// exceptions
namespace excep{

	//!\brief	Thrown when data is detected thats from a version that is greater than what this version can handle.
	class fromTheFuture: public base_error{
	public:
		fromTheFuture(const char* pFunc, const unsigned int pLine) throw():
            base_error(pFunc, pLine)
        {
		    addInfo("I am from the future, and unable to load.");
		}
		virtual ~fromTheFuture() throw(){}

	};
}

///////////////////////////////////////////////////////////////////////////////////
// classes
namespace gt{

	//-------------------------------------------------------------------------------------
	//!\brief	A figment of your imagination! More specifically, it's the base class type
	//!			for all the funky new stuff you'll make.
	class cFigment: public cFigContext{
	public:

		//-----------------------------
		// Commands and plug tags
		static const cPlugTag*	xPT_serialBuff;	//!< A smart pointer to the buffer where we load from, and save to.
		static const cPlugTag* xPT_loadingParty;	//!< This is a special group of figments relevant to loading.

		static const cCommand::dUID	xSave;	//!< Serialization is a base level ability. Expects a xPT_serialBuff.
		static const cCommand::dUID	xLoad;	//!< Expects a xPT_serialBuff and xPT_loadingParty

		//-----------------------------
		// Implemented
		cFigment();
		virtual ~cFigment();

		//!\brief	Jack is your interface for using data with this figment. You shouldn't need to override this, but in case you do.
		virtual void jack(ptrLead pLead, cContext* pCon);

		void run(cContext* pCon);	//!< Performs all the normal stuff needed before doing work, such as using the context to ensure it doesn't run into itself or other threads.

		//-----------------------------
		// These things are REQUIRED for any figment class.

		//!\brief	Used to define the string name of this object. It is also hashed to give the unique number
		//!			used to quickly compare objects.
		//!\note	You MUST replace this to identify your own
		static const dPlaChar* identify(){ return "figment"; }

		//- Not sure how I could eliminate this. I should be able to.
		virtual const dPlaChar* name() const { return identify(); }		//!< Virtual version of identify.
		virtual dNameHash hash() const { return getHash<cFigment>(); }

		//-----------------------------
		// standard interface. These are all optional in later classes.

		virtual void work(cContext* pCon);	//!< Gives the child figments some runtime to do whatever it is that they normally do.

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

		//!\brief	Version number used when loading. 0 Means that this version has no member plugs to load.
		//!\note	Migration is demo-ed in the testMigration class.
		static dNumVer version(){ return 0; }
		virtual dNumVer getVersion() const { return version(); }


		virtual dStr const & requiredAddon() const;	//!<	Unless this figment comes from an addon, only an empty string should be returned.
		virtual dMigrationPattern getLoadPattern();	//!< Load patterns offer you a way to migrate an older version of a figment to the current version. Override this function to pass back different load patterns. \note NOT threadsafe.
		virtual void getLinks(std::list<ptrFig>* pOutLinks);	//!< Append the list being passed in, with any figment pointers which form the run structure of the program. !\note NOT threadsafe.
		virtual void save(cByteBuffer* pSaveHere);	//!< Override this if you require special loading that a load pattern can't handle. !\note NOT threadsafe.
		virtual void loadEat(cByteBuffer* pLoadFrom, dReloadMap *aReloads = NULL); //!< Override this if you require special loading that a load pattern can't handle. !\note NOT threadsafe.


	protected:

		//-----------------------------
		// Patch through functions for use with command.
		void patSave(ptrLead aLead);	//!< Allows you to call the save function using jack
		void patLoad(ptrLead aLead);	//!< Same as the patSave function above.

	private:
		ptrLead tmpLead;	//!< Avoids mem alloc.
	};

	//-------------------------------------------------------------------------------------
	//!\brief	Designed to be the null return type. It doesn't do anything and it allows
	//!			objects to still function by referencing this dummy instead of null.
	//!\note	This is an example of the bare minimum that you have to do in order to make
	//!			a new figment type class.
	class cEmptyFig: public cFigment{
	public:
		cEmptyFig();
		virtual ~cEmptyFig();

		static const dPlaChar* identify(){ return "empty figment"; }
		virtual const dPlaChar* name() const{ return cEmptyFig::identify(); }
		virtual dNameHash hash() const{ return getHash<cEmptyFig>(); }
	};

	//-------------------------------------------------------------------------------------
	//!\brief	Use this object to cause the program to no longer loop, and so exit. However,
	//!			this is not an instant shutdown because the program must still finish the loop
	//!			it is on. Simply run it to shutoff the loop.
	//!\note	Should be called Unicron.
	class cWorldShutoff: public cFigment{
	public:
		cWorldShutoff();
		virtual ~cWorldShutoff();

		static const dPlaChar* identify(){ return "world shutoff"; }
		virtual const dPlaChar* name() const{ return identify(); }
		virtual dNameHash hash() const{ return getHash<cWorldShutoff>(); }

		virtual void work(cContext* pCon);
	};

}

///////////////////////////////////////////////////////////////////////////////////
// template specializations
namespace gt{

	//-----------------------------------------------------------------------------------
	//!\brief	This thing is slightly odd. I say odd as opposed to dangerous because
	//!			a figment pointer plug is only threadsafe because the interface to
	//!			figment is. Plugs use shadows to be threadsafe, but only if the contained
	//!			data doesn't have a reference that's outside a copy. Put another way, if
	//!			your plug is for a class that has a smart pointer, what that smart
	//!			pointer refers to won't be protected by the shadow strategy.
	template<>
	class tPlug<ptrFig>: public tPlugParent<ptrFig>
	{
	public:
		typedef void (*fuCopyInto)(const ptrFig *copyFrom, void *copyTo);
		typedef std::map<cBase_plug::dPlugType, fuCopyInto> dMapCopiers;

		tPlug():
			tPlugParent<ptrFig>(cBase_plug::getPlugType<ptrFig>())
		{
			mD = gWorld.get()->getEmptyFig();
		}

		tPlug(const ptrFig& pA):
			tPlugParent<ptrFig>(cBase_plug::getPlugType<ptrFig>())
		{
			mD = pA;
		}

		tPlug(const tPlug<ptrFig> &other):
			tPlugParent<ptrFig>(cBase_plug::getPlugType<ptrFig>())
		{
			mD = other.mD;
		}

		tPlug(const cBase_plug *other):
			tPlugParent<ptrFig>(cBase_plug::getPlugType<ptrFig>())
		{
			other->copyInto(&mD);
		}

		virtual ~tPlug(){}

		virtual cBase_plug& operator= (const cBase_plug &pD){
			NOTSELF(&pD);
			pD.copyInto(&mD);
			return *this;
		}

		virtual bool operator== (const cBase_plug &pD) const { return false; }

		cBase_plug& operator= (const tPlug<ptrFig> &other){
			NOTSELF(&other);
			mD = other.mD;
			return *this;
		}

		cBase_plug& operator= (const ptrFig& pA){
			mD = pA;
			return *this;
		}

		virtual void save(cByteBuffer* pAddHere){
			PROFILE;

			//- Using the pointer as a unique number to identify the referenced figment.
			dFigSaveSig saveSig = reinterpret_cast<dFigSaveSig>( get().get() );
			pAddHere->add( (dByte*)(&saveSig), sizeof(dFigSaveSig) );
		}

		virtual void loadEat(cByteBuffer* pChewToy, dReloadMap *aReloads){
			PROFILE;

			dFigSaveSig saveSig = 0;
			pChewToy->fill(&saveSig);
			pChewToy->trimHead(sizeof saveSig);

			dReloadMap::iterator itr = aReloads->find(saveSig);

			if(itr == aReloads->end())
				throw excep::notFound("signature of reloaded figment", __FILE__, __LINE__);	//- figment remains empty.

			get() = itr->second->fig;
		}

		virtual ptrFig& get(){
			return mD;
		}

		virtual const ptrFig& getConst() const{
			return mD;
		}

	protected:
		virtual void actualCopyInto(void* pContainer, cBase_plug::dPlugType pType) const{
			dMapCopiers::iterator itrCopiers = getPlugCopiers<ptrFig>()->find(getPlugType<ptrFig>());
			if(itrCopiers != getPlugCopiers<ptrFig>()->end()){
				itrCopiers->second( &mD, pContainer );
			}else{
				throw excep::cantCopy(typeid(ptrFig).name(), "unknown", __FILE__, __LINE__);
			}
		}

		virtual void actualCopyFrom(const void* pContainer, cBase_plug::dPlugType pType){
			PROFILE;

			if(pType != tPlugParent<ptrFig>::mType)
				throw excep::cantCopy(typeid(ptrFig).name(), "unknown", __FILE__, __LINE__);

			mD = *reinterpret_cast<const ptrFig*>(pContainer);
		}

	private:
		ptrFig mD;	//!< Data


	};

}


#ifdef GTUT
	namespace gt{
		//!\brief	Used to run a series of standard tests.
		template<typename FIGTYPE>
		void figmentTestSuit(){
			tOutline<FIGTYPE>::draft();
			tPlug<ptrFig> me = gWorld.get()->makeFig(getHash<FIGTYPE>());
			//tOutline<FIGTYPE>::removeFromWorld();
		}
	}
#endif

#endif
