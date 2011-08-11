#ifndef BLUEPRINT_HPP
#define BLUEPRINT_HPP

#include "lead.hpp"

///////////////////////////////////////////////////////////////////////////////////
// Constants
namespace gt{
	const dNameHash uDoesntReplace = 0;
}

///////////////////////////////////////////////////////////////////////////////////
// Classes
namespace gt{
	typedef std::map<dNameHash, cCommand> dComContainer;
	typedef std::map<dNameHash, cPlugTag> dPTagContainer;

	//---------------------------------------------------------------------------------------------------
	//!\brief	Blueprint for a figment
	class cBlueprint{
	public:

		cBlueprint();
		~cBlueprint();

		template<typename T> void setup();

		ptrFig make() const;
		dNameHash hash() const;
		const char* name() const;
		dNameHash replace() const;
		const cCommand* getCom(dNameHash pHash) const;
		const cPlugTag* gecPlugTag(dNameHash pPT) const;
		void addToGivenContainers(dComContainer* pComContain, dPTagContainer* pPTagContain) const;
		const cBlueprint* operator = (const cBlueprint* pCopy);

	private:
		dNameHash mHash;
		dNameHash mReplaces;
		ptrFig (*mFuncMake)();
		const cCommand* (*mGetCom)(dNameHash);
		const cPlugTag* (*mGecPlugTag)(dNameHash);
		const char* (*mGetName)();
		const dComContainer* mComContainRef;
		const dPTagContainer* mPTagContainRef;

		template<typename T> static ptrFig maker();
	};

	//---------------------------------------------------------------------------------------------------
	//!\brief	The outline of an object is used to manage the figment's blueprint, commands and plug tags.
	template<typename T>
	class tOutline{
	public:
		static void draft();	//!< Adds your figment to the world library.
		static void removeFromWorld();

		static const cCommand* makeCommand(
			const dNatChar* pName,
			unsigned int pSwitch,
			const cPlugTag* pTags,
			...
		);

		static const cPlugTag* makePlugTag(const dNatChar* pName);
		static const cCommand* getCommand(dNameHash pCommandID);
		static const cPlugTag* gecPlugTag(dNameHash pPTagID);

	protected:
		static cBlueprint xBlueprint;
		static dComContainer* xCommands;
		static dPTagContainer* xPlugTags;

		tOutline();
		~tOutline();

		dNameHash hash() const;

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
		DBUG_LO("making from blueprint, a " << T::identify());

		ptrFig temp(new T());
		return temp;
	}

	template<typename T>
	void
	cBlueprint::setup(){
		mHash = getHash<T>();
		mReplaces = T::replaces();
		mFuncMake = &(maker<T>);
		mGetCom = &(tOutline<T>::getCommand);
		mGecPlugTag = &(tOutline<T>::gecPlugTag);
		mGetName = &(T::identify);
		mComContainRef = tOutline<T>::xCommands;
		mPTagContainRef = tOutline<T>::xPlugTags;

		DBUG_LO("blueprint '" << T::identify() << "' defined.");
	}

	//--------------------------------------------------------
	// outline
	template<typename T>
	cBlueprint tOutline<T>::xBlueprint;

	template<typename T>
	dComContainer* tOutline<T>::xCommands;	//- Don't assign anything here.

	template<typename T>
	dPTagContainer* tOutline<T>::xPlugTags;	//- Don't assign anything here.

	template<typename T>
	bool tOutline<T>::xDrafted = false;

	template<typename T>
	void
	tOutline<T>::readyCommands(bool pDontCleanup){
		static bool setup = false;
		if(!pDontCleanup){	// Double negative bitches!
			if(setup){
				delete xCommands;
				xCommands = NULL;
				setup=false;
				DBUG_LO("Deleteing commands for " << T::identify());
			}
		}else{
			if(!setup){
				xCommands = new dComContainer();
				setup = true;
				DBUG_LO(T::identify() << " readied commands");
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
				xPlugTags = new dPTagContainer();
				setup = true;
				DBUG_LO(T::identify() << " readied plug tags");
			}
		}
	}

	template<typename T>
	tOutline<T>::tOutline(){
	}

	template<typename T>
	tOutline<T>::~tOutline(){

		//!\todo Figure out a way to run this when the outline is no longer needed.
		//readyCommands(false);
		//readyTags(false);
	}

	template<typename T>
	void
	tOutline<T>::draft(){
		if(!xDrafted){

			//T::requirements();	// (currently not working) Make sure that this figment has all the others it needs drafted in.

			readyCommands(); // You can't be sure a makeCommand has been run.
			readyTags(); // just in case.
			xBlueprint.setup<T>();

			//- If this replaces a figment, it must also copy some of its properties.
			if(T::replaces() != uDoesntReplace){
				const cBlueprint* temp = gWorld.get()->getBlueprint(T::replaces());
				temp->addToGivenContainers(xCommands, xPlugTags);
			}

			gWorld.get()->addBlueprint(&xBlueprint);

			xDrafted=true;

		}else{
			DBUG_LO("already drafted " << T::identify());
		}
	}

	template<typename T>
	void
	tOutline<T>::removeFromWorld(){
		if(xDrafted){
			gWorld.get()->removeBlueprint(&xBlueprint);
			xDrafted=false;
		}
	}

	template<typename T>
	dNameHash
	tOutline<T>::hash() const{
		PROFILE;
		return xBlueprint.hash();
	}

	template <typename T>
	const cCommand*
	tOutline<T>::makeCommand(
		const char* pName,
		unsigned int pSwitch,
		const cPlugTag* pTags,
		...
	){
		PROFILE;

		readyCommands();

		{	//- Using braces to ensure that the pointer to the command map is correct.
			dNameHash comUID = makeHash(pName);
			dComContainer::iterator itrCom;
			cPlugTag* param=const_cast<cPlugTag*>(pTags);
			va_list params;

			xCommands->insert( dComContainer::value_type(
					comUID,
					cCommand(comUID, pName, getHash<T>(), pSwitch )
			) );

			itrCom = xCommands->find(comUID);	// Doing this so we can use it below.

			va_start(params, pTags);
			while(param != NULL){
				itrCom->second.addTag(param);
				param  = va_arg( params, cPlugTag* );
			}
			va_end(params);

			DBUG_LO( "The outline of '" << T::identify() << "' made command '" << pName << "'");

			return &itrCom->second;
		}
	}

	template<typename T>
	const cPlugTag*
	tOutline<T>::makePlugTag(const dNatChar* pName){
		dPTagContainer::iterator itrTag;

		PROFILE;

		DBUG_LO("Making plug tag '" << pName << "' for figment '" << T::identify() << "'");

		readyTags();

		{
			dNameHash tagUID = makeHash(pName);

			xPlugTags->insert( dPTagContainer::value_type(
				tagUID,	cPlugTag(pName)
			) );

			itrTag = xPlugTags->find(tagUID);
		}

		return &itrTag->second;
	}

	template<typename T>
	const cCommand*
	tOutline<T>::getCommand(dNameHash pHash){
		dComContainer::iterator itrCom;

		PROFILE;

		readyCommands();

		itrCom = xCommands->find(pHash);

		if(itrCom == xCommands->end())
			throw excep::notFound("can't find command", __FILE__, __LINE__);

		return &itrCom->second;
	}

	template<typename T>
	const cPlugTag*
	tOutline<T>::gecPlugTag(dNameHash pHash){
		dPTagContainer::iterator itrTag;

		PROFILE;

		readyTags();

		itrTag = xPlugTags->find(pHash);

		if(itrTag == xPlugTags->end())
			throw excep::notFound("can't find plug tag", __FILE__, __LINE__);

		return &itrTag->second;
	}

}

#endif
