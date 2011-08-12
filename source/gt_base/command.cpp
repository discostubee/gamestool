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
cCommand::usesTag(const tPlugTag* pTag) const{
	if( mDataTags.find(pTag) != mDataTags.end() )
		return true;

	return false;
}

void
cCommand::addTag(const tPlugTag* pTag){
	ASRT_NOTNULL(pTag);

	mDataTags.insert(pTag);
}

cCommand&
cCommand::operator=(const cCommand& pCom){
	DONT_USE_THIS;
	return *this;
}

////////////////////////////////////////////////////////////
// Tests
#ifdef GTUT

class cComTestFig: public cFigment, private tOutline<cComTestFig>{
public:
	static const tPlugTag* xPT_testTag;

	static const dNatChar* identify(){ return "command test figment"; }

	enum{
		eTestSwitch = cFigment::eSwitchEnd +1,
		eSwitchEnd
	};

	cComTestFig(){}
	~cComTestFig(){}
};

const tPlugTag* cComTestFig::xPT_testTag = tOutline<cComTestFig>::makePlugTag("test tag");

GTUT_START(command, nothing)
{
	const cCommand* A = tOutline<cComTestFig>::makeCommand("test command", cComTestFig::eTestSwitch, cComTestFig::xPT_testTag);
	GTUT_ASRT( A->getSwitch<cComTestFig>() ==  cComTestFig::eTestSwitch, "command switch is not correct" );
}
GTUT_END;

#endif
