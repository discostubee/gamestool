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

#include "outline.hpp"
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
// macros

//!\brief	A helpful way to reducing pointless busy work.
#define GT_IDENTIFY(x)\
		static const dPlaChar* identify(){ return x; }\
		virtual const dPlaChar* name() const { return identify(); }

//!\brief	This is different to the normal c++ class inheritance because it gives you class the ability to accept commands
//!			from its parent. So in this sense, you don't always need to exten a class.
//!\note	A helpful way to reduce pointless busy work.
#define GT_EXTENDS(x)\
		static dNameHash extends(){ return getHash<x>(); }\
		virtual dNameHash getExtension() const { return extends(); }

//!\brief	When this figment is drafted, it replaces the given one in the world. That means that when you request the old
//!			figment, you get the new one instead.
//!\note	A helpful way to reduce pointless busy work.
#define GT_REPLACES(x)\
		static dNameHash replaces(){ return getHash<x>(); }\
		virtual dNameHash getReplacement() const{ return replaces(); }

//!\brief	Using version (with a number above 0) means you have stuff you want to save. So you'll need to implement
//!			the dMigrationPattern getLoadPattern() function
//!\note	A helpful way to reduce pointless busy work.
#define GT_VERSION(x)\
		static dNumVer version(){ return x; }\
		virtual dNumVer getVersion() const { return version(); }\
		virtual dMigrationPattern getLoadPattern()


///////////////////////////////////////////////////////////////////////////////////
// types
typedef boost::shared_ptr<cByteBuffer> ptrBuff;


///////////////////////////////////////////////////////////////////////////////////
// classes
namespace gt{

	//-------------------------------------------------------------------------------------
	//!\brief	A figment of your imagination! More specifically, it's the base class type
	//!			for all the funky new stuff you'll make.
	class cFigment: public cFigContext{
	public:

		//--- Commands and plug tags
		static const cPlugTag* xPT_serialBuff;	//!< A smart pointer to the buffer where we load from, and save to.
		static const cPlugTag* xPT_loadingParty;	//!< This is a special group of figments relevant to loading.
		static const cPlugTag* xPT_name;
		static const cPlugTag* xPT_hash;
		static const cPlugTag* xPT_commands;	//!< An array plug to contain
		static const cPlugTag* xPT_links;	//!< An array of plug to contain all the figments.

		static const cCommand::dUID	xSave;	//!< Serialization is a base level ability. Expects a xPT_serialBuff.
		static const cCommand::dUID	xLoad;	//!< Expects a xPT_serialBuff and xPT_loadingParty
		static const cCommand::dUID	xGetName;		//!< Fills a provided plug with the figment's name.
		static const cCommand::dUID	xGetHash;		//!< Fills a provided plug with the hash of the figment's name.
		static const cCommand::dUID	xGetCommands;	//!< Gets a list of all the commands used by this figment.
		static const cCommand::dUID	xGetLinks;		//!< Fills a provided container with the list of links found using getLinks

		//--- Implemented
		cFigment();
		virtual ~cFigment();

		//!\brief	Jack is your interface for using data with this figment.
		virtual void jack(ptrLead pLead, cContext* pCon);

		void run(cContext* pCon);	//!< Performs all the normal stuff needed before doing work, such as using the context to ensure it doesn't run into itself or other threads.
		ptrFig getSmart();

		//--- These things are REQUIRED for any figment class.

		//!\brief	Used to define the string name of this object. It is also hashed to give the unique number
		//!			used to quickly compare objects.
		//!\note	You MUST replace this to identify your own
		static const dPlaChar* identify(){ return "figment"; }

		virtual const dPlaChar* name() const { return identify(); }		//!< Virtual version of identify.
		virtual dNameHash hash() const { return getHash<cFigment>(); }

		//--- standard interface. These are all optional in later classes.

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

		virtual void work(cContext* pCon);	//!< Gives the child figments some runtime to do whatever it is that they normally do.
		virtual dStr const & requiredAddon() const;	//!< Unless this figment comes from an addon, only an empty string should be returned.
		virtual dMigrationPattern getLoadPattern();	//!< Load patterns offer you a way to migrate an older version of a figment to the current version. Override this function to pass back different load patterns. \note NOT threadsafe.
		virtual void getLinks(std::list<ptrFig> *pOutLinks);	//!< Append the list being passed in, with any figment pointers which form the run structure of the program. !\note NOT threadsafe.
		virtual void save(cByteBuffer* pSaveHere);	//!< Override this if you require special loading that a load pattern can't handle. !\note NOT threadsafe.
		virtual void loadEat(cByteBuffer* pLoadFrom, dReloadMap *aReloads = NULL); //!< Override this if you require special loading that a load pattern can't handle. !\note NOT threadsafe.

	protected:

		//- Patch through functions for use with command.
		void patSave(ptrLead aLead);	//!< Allows you to call the save function using jack
		void patLoad(ptrLead aLead);	//!< Same as the patSave function above.
		void patGetName(ptrLead aLead);
		void patGetHash(ptrLead aLead);
		void patGetCommands(ptrLead aLead);
		void patGetLinks(ptrLead aLead);

		void ini(cBlueprint *pBlue, tDirector<iFigment> *pSelf);

	private:
		cBlueprint *mBlueprint;	//!< This should last longer than any figment.
		tDirector<iFigment> *mSelf;
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

		GT_IDENTIFY("empty figment");
		virtual dNameHash hash() const{ return getHash<cEmptyFig>(); }
	};

	//-------------------------------------------------------------------------------------
	//!\brief	You've get the touch. Use this object to cause the program to no longer
	//!			loop, and so exit. However, this is not an instant shutdown because the
	//!			program must still finish the loop it is on.
	class cUnicron: public cFigment{
	public:
		cUnicron();
		virtual ~cUnicron();

		GT_IDENTIFY("unicron");
		virtual dNameHash hash() const{ return getHash<cUnicron>(); }

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
	class tPlugFlakes<ptrFig>: public tDataPlug<ptrFig>{
	public:
		//---
		virtual ptrFig& get() = 0;
		virtual const ptrFig& getConst() const =0;

		//---
		virtual void save(cByteBuffer* pAddHere){
			PROFILE;

			//- Using the pointer as a unique number to identify the referenced figment.
			dFigSaveSig saveSig = 0;
			if(get().valid())
				saveSig = reinterpret_cast<dFigSaveSig>( get().get() );

			pAddHere->add( (dByte*)(&saveSig), sizeof(dFigSaveSig) );
		}

		virtual void loadEat(cByteBuffer* pChewToy, dReloadMap *aReloads){
			PROFILE;

			dFigSaveSig saveSig = 0;
			pChewToy->fill(&saveSig);
			pChewToy->trimHead(sizeof saveSig);

			if(saveSig == 0)
				return;

			dReloadMap::iterator itr = aReloads->find(saveSig);

			if(itr == aReloads->end())
				throw excep::notFound("signature of reloaded figment", __FILE__, __LINE__);	//- figment remains empty.

			get() = itr->second->fig;
		}

		virtual bool operator== (const cBase_plug &pD) const {
			return (pD.getType() == genPlugType<ptrFig>());
		}
	};

	//-----------------------------------------------------------------------------------
	template<>
	class cAnyOp::tOps<ptrBuff>{
	private:
		static void assign(const ptrBuff & pFrom, void *pTo){
		}

		static void append(const ptrBuff & pFrom, void *pTo){
		}

	public:
		static void setup(tKat<ptrBuff> * pK, cAnyOp * pUsing){
			pK->addAss(&getRef(), genPlugType<ptrBuff>(), assign);
			pK->addApp(&getRef(), genPlugType<ptrBuff>(), append);
		}
	};

}

#endif
