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
	//!\brief	A command defines what input/output functions a class can use.
	class cCommand{
	public:
		typedef unsigned int dUID;	//!< unique command ID

		const dUID	mID;
		const dStr	mName;

		cCommand(
			const dUID pID,
			const dNatChar* pName,
			const dNameHash pParentHash,
			unsigned int pSwitch
		);

		~cCommand();

		template<typename T>
		unsigned int getSwitch() const;	//!< Gives you the switch, but only if you are the figment it is looking for.

		bool usesTag(const tPlugTag* pTag) const;	//!< Checks if it has such a tag.
		void addTag(const tPlugTag* pTag);

		cCommand& operator=(const cCommand& pCom);

	private:
		const dNameHash mParent;	//!< Used to determine which figment this command belongs too.
		const unsigned int mSwitch;

		std::set<const tPlugTag*> mDataTags;
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

////////////////////////////////////////////////////////////////////
// Template methods
namespace gt{

	template<typename T>
	unsigned int
	cCommand::getSwitch() const{
		if (mParent == getHash<T>())
			return mSwitch;
		else
			return uNotMyBag;
	}
}

#endif
