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
		typedef void (iFigment::*fooPtr)(cLead *aLead);	//!< Pointer to our jack function.

		const dUID	mID;
		const dStr	mName;
		const dNameHash mParent;	//!< Used to determine which figment this command belongs too.
		fooPtr myFoo;

		cCommand(
			const dUID pID,
			const char* pName,
			const dNameHash pParentHash,
			fooPtr aFoo
		);

		virtual ~cCommand();

		bool usesTag(const cPlugTag* pTag) const;	//!< Checks if it has such a tag.
		void addTag(const cPlugTag* pTag);

		void use(iFigment *aFig, cLead *aLead) const;	//!< Assumes you've already checked the figment and the command match up.

		cCommand& operator=(const cCommand& pCom);

	private:
		std::set<const cPlugTag*> mDataTags;
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

#endif
