/**********************************************************************************************************
 * !\file	command.hpp
 * !\brief
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

#ifndef COMMAND_HPP
#define COMMAND_HPP

#include "world.hpp"


///////////////////////////////////////////////////////////////////////////////////
// constants
namespace gt{
	const unsigned int uNotMyBag=0;
}

///////////////////////////////////////////////////////////////////////////////////
namespace gt{

	//----------------------------------------------------------------------------
	//!\brief	A command defines how the generic jack interface is used by a figment.
	//!			That means linking to the callback function, defining what figment
	//!			uses it, as well as what tagged data this command uses.
	class cCommand{
	public:
		typedef unsigned int dUID;	//!< unique command ID

		static const dUID noID;

		const dUID	mID;
		const dStr	mName;
		const dNameHash mParent;	//!< Used to determine which figment this command belongs too. Not using a callback from the actual command so that extending classes can make use of this command when respawned.

		cCommand(
			const dUID pID,
			const char* pName,
			const dNameHash pParentHash
		);

		virtual ~cCommand();

		virtual bool usesTag(const cPlugTag* pTag) const =0;	//!< Checks if it has such a tag.
		virtual void addTag(const cPlugTag* pTag) =0;
		virtual void use(iFigment *aFig, ptrLead aLead) const =0;	//!<
		virtual cCommand *respawn(dNameHash diffParent) const =0;	//!< Allows you to create a copy of this command but with a different parent.
	};

	//----------------------------------------------------------------------------
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

///////////////////////////////////////////////////////////////////////////////////
// Template functions
namespace gt{

	//!\brief	Generates a hash for this figment and remembers is, so it won't regenerate it every time.
	template <typename T>
	dNameHash
	getHash(){
		static dNameHash name = 0;
		if(name == 0)
			name = ::makeHash( toNStr(T::identify()) );
		return name;
	}
}

////////////////////////////////////////////////////////////
// template definitions
namespace gt{

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
			( dynamic_cast<T*>(aFig)->*myFoo )(aLead);
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
