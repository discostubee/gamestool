/*
 * !\file	command.hpp
 * !\brief
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
		const dNameHash mParent;	//!< Used to determine which figment this command belongs too.

		cCommand(
				const dUID pID,
				const char* pName,
				const dNameHash pParentHash
		);

		virtual ~cCommand();

		virtual bool usesTag(const cPlugTag* pTag) const =0;	//!< Checks if it has such a tag.
		virtual void addTag(const cPlugTag* pTag) =0;
		virtual void use(iFigment *aFig, ptrLead aLead) const =0;	//!<

	};

	//----------------------------------------------------------------------------
	template<typename T>
	class tActualCommand : public cCommand{
	public:
		typedef void (T::*ptrPatFoo)(ptrLead aLead);	//!< Pointer to our jack function.

		tActualCommand(
			const dUID pID,
			const char* pName,
			const dNameHash pParentHash,
			ptrPatFoo aFoo
		);

		virtual ~tActualCommand();

		virtual bool usesTag(const cPlugTag* pTag) const;	//!< Checks if it has such a tag.
		virtual void addTag(const cPlugTag* pTag);
		virtual void use(iFigment *aFig, ptrLead aLead) const;

	private:
		std::set<const cPlugTag*> mDataTags;
		ptrPatFoo myFoo;
	};

}

///////////////////////////////////////////////////////////////////////////////////
// Template functions
namespace gt{

	//!\brief	Generates a hash for this class and remembers is, so it won't regenerate it every time.
	template <typename T>
	dNameHash
	getHash(){
		static dNameHash name = 0;
		if(name == 0){
			name = ::makeHash(T::identify());
		}
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
		if(aFig->hash() == getHash<T>())
			( dynamic_cast<T*>(aFig)->*myFoo )(aLead);
		else
			throw excep::base_error("can't use", __FILE__, __LINE__);
	}
}

#endif
