#include "command.hpp"
#include "figment.hpp"

////////////////////////////////////////////////////////////
using namespace gt;

cCommand::cCommand(
	const dUID pID,
	const dNatChar* pName,
	const dNameHash pParentHash,
	unsigned int pSwitch
):
	mID(pID), mName(pName), mParent(pParentHash), mSwitch(pSwitch)
{
}

cCommand::~cCommand(){
}


bool
cCommand::usesTag(const cPlugTag* pTag) const{
	if( mDataTags.find(pTag) != mDataTags.end() )
		return true;

	return false;
}

void
cCommand::addTag(const cPlugTag* pTag){
	ASRT_NOTNULL(pTag);

	mDataTags.insert(pTag);
}

cCommand&
cCommand::operator=(const cCommand& pCom){
	DONT_USE_THIS;
}
