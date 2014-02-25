/*
 * !\file		outline.hpp
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

#ifndef OUTLINE_HPP
#define OUTLINE_HPP

#include "world.hpp"


///////////////////////////////////////////////////////////////////////////////////
// Classes
namespace gt{


	//---------------------------------------------------------------------------------------------------
	//!\brief	The outline manages commands, plug tags and blueprints for a figment type.
	template<typename T>
	class tOutline{
	public:
		typedef void (T::*ptrPatFoo)(ptrLead alead);

		static void draft(const dStr &pAddon="");	//!< Adds your figment to the world library and increments the reference count. !\param pAddon (optional) name of the addon this blueprint is coming from.
		static void remove();	//!< Only do this when an addon closes or the main program ends

		//!\brief	Use a unique name and a pointer to a function, along with an optional, BUT ALWAYS NULL TERMINATED, list of plug tags to create a command.
		//!\note	The string name provided here, and the one for the class, is used to create a unique identifier. So make these names unique for the class.
		//!\note	Any and all provided tagged plugs are always assumed to be optional. Functions SHOULD NOT ERROR if arguments are not provided.
		static const cCommand::dUID makeCommand(
			const char* aName,
			ptrPatFoo aFoo,
			const cPlugTag* pTags = NULL,
			...
		);

		static const cPlugTag* makePlugTag(const dPlaChar* pName);			//!< Throws if the name isn't found in the list of tags for this outline.
		static const cCommand* getCommand(cCommand::dUID pCommandID);	//!< Throws if the command not found.
		static const cPlugTag* getPlugTag(cPlugTag::dUID pPTagID);		//!< Returns reference, or NULL. Does not throw.
		static dListComs getAllCommands();
		static dListPTags getAllTags();
		static dExtensions getExtensions();
		static bool hasPlugTag(cPlugTag::dUID pPTID);

	protected:
		typedef std::map<cCommand::dUID, cCommand* > dMapCom;
		typedef std::map<cPlugTag::dUID, cPlugTag> dMapPTag;

		static cBlueprint xBlueprint;	//!< The blueprint formed by this outline.
		static dExtensions xExtensions;	//!< This is the list of blueprints that this outline can use commands and tags from. If you replace a figment you automatically take that blueprint as an extension.
		static dMapCom* xCommands;
		static dMapPTag* xPlugTags;
		static bool xDrafted;
		static dStr xAddon;	//!< Addon this outline may have come from.

		tOutline();
		~tOutline();

		static ptrFig makeFig();
		static void unmakeFig();

		friend class cBlueprint;

	private:
		static void readyCommands(bool pDontCleanup=true);	//!<
		static void readyTags(bool pDontCleanup=true);
	};

	//----------------------------------------------------------------------------
	//!\brief
	template<typename T>
	class tActualCommand : public cCommand{
	public:
		typedef void (T::*ptrPatFoo)(ptrLead aLead);	//!< Pointer to our patch through function.

		tActualCommand(
			const dUID pID,
			const char* pName,
			const dNameHash pParentHash,
			ptrPatFoo aFoo
		);

		virtual ~tActualCommand();

		virtual bool usesTag(const cPlugTag* pTag) const;
		virtual void addTag(const cPlugTag* pTag);
		virtual void use(iFigment *aFig, ptrLead aLead) const;
		virtual cCommand *respawn(dNameHash diffParent) const;

	private:
		std::set<const cPlugTag*> mDataTags;
		ptrPatFoo myFoo;
	};


}

////////////////////////////////////////////////////////////////////
// Templates
namespace gt{

	//--------------------------------------------------------
	// outline
	template<typename T>
	cBlueprint tOutline<T>::xBlueprint;

	template<typename T>
	dExtensions tOutline<T>::xExtensions;

	template<typename T>
	typename tOutline<T>::dMapCom *tOutline<T>::xCommands;	//- Don't assign anything here.

	template<typename T>
	typename tOutline<T>::dMapPTag *tOutline<T>::xPlugTags;	//- Don't assign anything here.

	template<typename T>
	bool tOutline<T>::xDrafted=false;

	template<typename T>
	dStr tOutline<T>::xAddon;


	template<typename T>
	void
	tOutline<T>::readyCommands(bool pDontCleanup){
		static bool setup = false;
		if(!pDontCleanup){	// Double negative bitches!
			if(setup){
				for(dMapCom::iterator itr = xCommands->begin(); itr != xCommands->end(); ++itr)
					delete itr->second;
				delete xCommands;
				xCommands = NULL;
				setup=false;
				DBUG_VERBOSE_LO("Deleting commands for " << T::identify());
				DBUG_TRACK_END("commands");
			}
		}else{
			if(!setup){
				xCommands = new dMapCom();
				setup = true;
				DBUG_VERBOSE_LO("Made commands for " << T::identify());
				DBUG_TRACK_START("commands");
			}
		}
	}

	template<typename T>
	void
	tOutline<T>::readyTags(bool pDontCleanup){
		static bool setup = false;
		if(!pDontCleanup){
			if(setup){
				delete xPlugTags;
				xPlugTags = NULL;
				setup = false;
				DBUG_VERBOSE_LO("Deleting tags for " << T::identify());
				DBUG_TRACK_END("tags");
			}
		}else{
			if(!setup){
				xPlugTags = new dMapPTag();
				setup = true;
				DBUG_VERBOSE_LO("Made plug tags for " << T::identify());
				DBUG_TRACK_START("tags");
			}
		}
	}

	template<typename T>
	tOutline<T>::tOutline(){
	}

	template<typename T>
	tOutline<T>::~tOutline(){
	}

	template<typename T>
	void
	tOutline<T>::draft(const dStr &pAddon){
		if(xDrafted){
			WARN_S(T::identify() << " already drafted.");
			return;
		}

		xAddon = pAddon;

		readyCommands(); // You can't be sure a makeCommand has been run.
		readyTags(); // just in case.
		xBlueprint.install(
			getHash<T>(),
			T::replaces(),
			T::extends(),
			&makeFig,
			&unmakeFig,
			&(T::identify),
			&getCommand,
			&getPlugTag,
			&getAllCommands,
			&getAllTags,
			&getExtensions,
			&hasPlugTag,
			&remove
		);

		cBlueprint const *tmpBlue = NULL;

		if(xBlueprint.replace() != uDoesntReplace)
			tmpBlue = gWorld.get()->getBlueprint(xBlueprint.replace());
		else if(xBlueprint.extend() != uDoesntExtend)
			tmpBlue = gWorld.get()->getBlueprint(xBlueprint.extend());

		if(tmpBlue != NULL){
			for(dListComs cmds = tmpBlue->getAllComs(); !cmds.empty(); cmds.pop_front()){
				(void)xCommands->insert( dMapCom::value_type(
					cmds.front()->mID,
					cmds.front()->respawn(getHash<T>())
				) );

				DBUG_VERBOSE_LO(T::identify() << " copied command " << cmds.front()->mName );
			}

			for(dListPTags tags = getAllTags(); !tags.empty(); tags.pop_front()){
				xPlugTags->insert( dMapPTag::value_type(
					tags.front()->mID,
					*tags.front()
				) );

				DBUG_VERBOSE_LO(T::identify() << " copied tag " << tags.front()->mName );
			}

			xExtensions = tmpBlue->getExtensions();
			xExtensions.push_back(tmpBlue);
		}

		gWorld.get()->addBlueprint(&xBlueprint, pAddon);
		xDrafted = true;
	}

	template<typename T>
	void
	tOutline<T>::remove(){
		if(!xDrafted){
			WARN_S(T::identify() << " can't be removed, as it's not drafted.");
			return;
		}

		gWorld.get()->removeBlueprint(&xBlueprint, xAddon);
		readyCommands(false);
		readyTags(false);
	}

	template<typename T>
	ptrFig
	tOutline<T>::makeFig(){
		DBUG_VERBOSE_LO("making a " << T::identify());
		return ptrFig(new T());
	}

	template<typename T>
	void
	tOutline<T>::unmakeFig(){
	}

	template <typename T>
	const cCommand::dUID
	tOutline<T>::makeCommand(
		const dPlaChar* pName,
		ptrPatFoo aFoo,
		const cPlugTag* pTags,
		...
	){
		PROFILE;

		readyCommands();

		{	//- Using braces to ensure that the pointer to the command map is correct.
			dNameHash comUID;
			{
				dNatStr totalString = PCStr2NStr(T::identify());
				totalString.t.append( PCStr2NStr(pName).t );
				comUID = makeHash( totalString );
			}
			dMapCom::iterator itrCom;
			cPlugTag* param=const_cast<cPlugTag*>(pTags);
			va_list params;

			xCommands->insert( dMapCom::value_type(
				comUID,
				new tActualCommand<T>(comUID, pName, getHash<T>(), aFoo )
			) );

			itrCom = xCommands->find(comUID);	// Doing this so we can use it below.

			va_start(params, pTags);
			while(param != NULL){
				itrCom->second->addTag(param);
				param  = va_arg( params, cPlugTag* );
			}
			va_end(params);

			DBUG_VERBOSE_LO( "The outline of '" << T::identify() << "' made command '" << pName << "'");

			return comUID;
		}
	}

	template<typename T>
	const cPlugTag*
	tOutline<T>::makePlugTag(const dPlaChar* pName){
		PROFILE;

		DBUG_VERBOSE_LO("Making plug tag '" << pName << "' for figment '" << T::identify() << "'");

		readyTags();
		dMapPTag::iterator itrTag = xPlugTags->end();

		{
			dNameHash tagUID = makeHash( toNStr(pName) );

			xPlugTags->insert( dMapPTag::value_type(tagUID,	cPlugTag(pName)) );

			itrTag = xPlugTags->find(tagUID);
		}

		return &itrTag->second;
	}

	template<typename T>
	const cCommand*
	tOutline<T>::getCommand(cCommand::dUID pHash){
		dMapCom::iterator itrCom;

		PROFILE;

		readyCommands();

		itrCom = xCommands->find(pHash);

		if(itrCom == xCommands->end()){
			std::stringstream ss; ss << T::identify() << " command " << pHash;
			throw excep::notFound(ss.str().c_str(), __FILE__, __LINE__);
		}

		return itrCom->second;
	}

	template<typename T>
	const cPlugTag*
	tOutline<T>::getPlugTag(cPlugTag::dUID pHash){
		cPlugTag const *rtnTag = NULL;

		PROFILE;

		readyTags();

		dMapPTag::iterator itrTag = xPlugTags->find(pHash);

		if(itrTag == xPlugTags->end()){
			for(
				dExtensions::iterator itr = xExtensions.begin();
				itr != xExtensions.end() && rtnTag==NULL;
				++itr
			){
				rtnTag = (*itr)->getPlugTag(pHash);
			}
		}else{
			rtnTag = &itrTag->second;
		}

		return rtnTag;
	}

	template<typename T>
	dListComs
	tOutline<T>::getAllCommands(){
		dListComs rtnList;

		readyCommands();

		for(dMapCom::iterator itr = xCommands->begin(); itr != xCommands->end(); ++itr)
			rtnList.push_back(itr->second);

		return rtnList;
	}

	template<typename T>
	dListPTags
	tOutline<T>::getAllTags(){
		dListPTags rtnList;

		readyTags();

		for(dMapPTag::iterator itr = xPlugTags->begin(); itr != xPlugTags->end(); ++itr)
			rtnList.push_back(&itr->second);

		return rtnList;
	}

	template<typename T>
	dExtensions
	tOutline<T>::getExtensions(){
		return xExtensions;
	}


	template<typename T>
	bool
	tOutline<T>::hasPlugTag(cPlugTag::dUID pPTID){
		readyTags();

		dMapPTag::iterator found = xPlugTags->find(pPTID);
		return (found != xPlugTags->end());
	}

	//--------------------------------------------------------
	template<typename T>
	tActualCommand<T>::tActualCommand(
		const dUID pID,
		const char* pName,
		const dNameHash pParentHash,
		ptrPatFoo aFoo
	) :
		cCommand(pID, pName, pParentHash),
		myFoo(aFoo)
	{}

	template<typename T>
	tActualCommand<T>::~tActualCommand(){
	}

	template<typename T>
	bool
	tActualCommand<T>::usesTag(const cPlugTag* pTag) const{
		if( mDataTags.find(pTag) != mDataTags.end() )
			return true;

		return false;
	}

	template<typename T>
	void
	tActualCommand<T>::addTag(const cPlugTag* pTag){
		ASRT_NOTNULL(pTag);

		mDataTags.insert(pTag);
	}

	template<typename T>
	void
	tActualCommand<T>::use(iFigment *aFig, ptrLead aLead) const {
		if(aFig->hash() == mParent){
			( static_cast<T*>(aFig)->*myFoo )(aLead);
		}else{
			THROW_ERROR(aFig->name() << " can't use command " << mName << " (" << aFig->hash() << ", " << mParent << ")");
		}
	}

	template<typename T>
	cCommand*
	tActualCommand<T>::respawn(dNameHash diffParent) const {
		return new tActualCommand<T>(mID, mName.c_str(), diffParent, myFoo);
	}
}

#endif
