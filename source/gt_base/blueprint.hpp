/*
 * !\file		blueprint.hpp
 * !\brief		The blueprint file contains both the blueprint class and the figment outline template. This is because the 2
 * !			are pretty interdependent.
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

#ifndef BLUEPRINT_HPP
#define BLUEPRINT_HPP

#include "lead.hpp"

///////////////////////////////////////////////////////////////////////////////////
// Constants
namespace gt{
	const dNameHash uDoesntReplace = 0;
	const dNameHash uDoesntExtend = 0;
}

///////////////////////////////////////////////////////////////////////////////////
// Types
namespace gt{
	typedef std::list<const cCommand*> dListComs;
	typedef std::list<const cPlugTag*> dListPTags;
	typedef std::vector<const cBlueprint *> dExtensions;
}

///////////////////////////////////////////////////////////////////////////////////
// Classes
namespace gt{

	//---------------------------------------------------------------------------------------------------
	//!\brief	Blueprint for a figment, used by the world when making figments and is the interface to
	//!			to the outline.
	class cBlueprint{
	public:

		cBlueprint();
		~cBlueprint();

		template<typename T> void setup();

		ptrFig make();
		dNameHash hash() const;
		const dPlaChar* name() const;
		dNameHash replace() const;
		dNameHash extend() const;
		const cCommand* getCom(cCommand::dUID pHash) const;
		const cPlugTag* getPlugTag(cPlugTag::dUID pPT) const;
		dListComs getAllComs() const;
		dListPTags getAllTags() const;
		dExtensions getExtensions() const;
		bool hasPlugTag(cPlugTag::dUID pPT) const;

		const cBlueprint* operator = (const cBlueprint* pCopy);

	protected:

		void (*mCleanup)();	//!< Used when the world is destroyed.

	friend class cWorld;

	private:
		dNameHash mHash;
		dNameHash mReplaces;
		dNameHash mExtends;
		ptrFig (*mFuncMake)();
		const dPlaChar* (*mGetName)();
		const cCommand* (*mGetCom)(cCommand::dUID);
		const cPlugTag* (*mGetPlugTag)(cPlugTag::dUID);
		dListComs (*mGetAllComs)();
		dListPTags (*mGetAllTags)();
		dExtensions (*mGetExtensions)();
		bool (*mHasPlugTag)(cPlugTag::dUID);

		template<typename T> static ptrFig maker();
	};

	//---------------------------------------------------------------------------------------------------
	//!\brief	The outline provides an interface to help a figment manage its blueprint, commands and plug tags
	template<typename T>
	class tOutline{
	public:
		typedef void (T::*ptrPatFoo)(ptrLead alead);

		static void draft();				//!< Adds your figment to the world library.
		static void removeFromWorld();

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

		tOutline();
		~tOutline();

		static void cleanup();

		friend class cBlueprint;

	private:
		static bool xDrafted;

		static void readyCommands(bool pDontCleanup=true);	//!<
		static void readyTags(bool pDontCleanup=true);
	};

}

////////////////////////////////////////////////////////////////////
// Templates
namespace gt{

	//--------------------------------------------------------
	// blueprint
	template<typename T>
	ptrFig
	cBlueprint::maker(){
		DBUG_VERBOSE_LO("making from blueprint, a " << T::identify());

		ptrFig temp(new T());
		temp->self = temp.getDir();
		return temp;
	}

	template<typename T>
	void
	cBlueprint::setup(){
		mHash = getHash<T>();
		mReplaces = T::replaces();
		mExtends = T::extends();
		mFuncMake = &(maker<T>);
		mGetName = &(T::identify);
		mGetCom = &(tOutline<T>::getCommand);
		mGetPlugTag = &(tOutline<T>::getPlugTag);
		mGetAllComs = &(tOutline<T>::getAllCommands);
		mGetAllTags = &(tOutline<T>::getAllTags);
		mGetExtensions = &(tOutline<T>::getExtensions);
		mHasPlugTag = &(tOutline<T>::hasPlugTag);
		mCleanup = &(tOutline<T>::cleanup);

		DBUG_LO("blueprint '" << T::identify() << "' defined.");
	}

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
	bool tOutline<T>::xDrafted = false;



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
				DBUG_LO("Deleting commands for " << T::identify());
			}
		}else{
			if(!setup){
				xCommands = new dMapCom();
				setup = true;
				DBUG_VERBOSE_LO(T::identify() << " readied commands");
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
				DBUG_LO("Deleting tags for " << T::identify());
			}
		}else{
			if(!setup){
				xPlugTags = new dMapPTag();
				setup = true;
				DBUG_VERBOSE_LO(T::identify() << " readied plug tags");
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
	tOutline<T>::draft(){
		if(!xDrafted){
			readyCommands(); // You can't be sure a makeCommand has been run.
			readyTags(); // just in case.
			xBlueprint.setup<T>();

			cBlueprint const *tmpBlue = NULL;

			if(xBlueprint.replace() != uDoesntReplace)
				tmpBlue = gWorld.get()->getBlueprint(xBlueprint.replace());
			else if(xBlueprint.extend() != uDoesntExtend)
				tmpBlue = gWorld.get()->getBlueprint(xBlueprint.extend());

			if(tmpBlue != NULL){
				for(dListComs cmds = tmpBlue->getAllComs(); !cmds.empty(); cmds.pop_front()){
					xCommands->insert( dMapCom::value_type(
						cmds.front()->mID,
						cmds.front()->respawn(getHash<T>())
					) );

					DBUG_VERBOSE_LO(T::identify() << " respawned command " << cmds.front()->mName );
				}

				xExtensions = tmpBlue->getExtensions();
				xExtensions.push_back(tmpBlue);
			}

			gWorld.get()->addBlueprint(&xBlueprint);

			xDrafted=true;

		//}else{
		//	WARN_S("already drafted " << T::identify());
		}
	}

	template<typename T>
	void
	tOutline<T>::removeFromWorld(){
		cleanup();
		gWorld.get()->removeBlueprint(&xBlueprint);
	}

	template<typename T>
	void
	tOutline<T>::cleanup(){
		if(xDrafted){
			readyCommands(false);
			readyTags(false);

			xDrafted=false;
		}
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
				dNatStr totalString = toNStr(T::identify());
				totalString.t.append( toNStr(pName).t );
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

		dMapPTag::iterator itrTag = xPlugTags->end();

		DBUG_VERBOSE_LO("Making plug tag '" << pName << "' for figment '" << T::identify() << "'");

		readyTags();

		{
			dNameHash tagUID = makeHash( toNStr(pName).t.c_str() );

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
}

#endif
