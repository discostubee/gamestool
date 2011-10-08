#include "command.hpp"
#include "figment.hpp"

////////////////////////////////////////////////////////////
using namespace gt;

cCommand::cCommand(
	const dUID pID,
	const char* pName,
	const dNameHash pParentHash,
	fooPtr aFoo
):
	mID(pID), mName(pName), mParent(pParentHash), myFoo(aFoo)
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
	DUMB_REF_ARG(pCom); DONT_USE_THIS;
	//return *this;
}

void
cCommand::use(iFigment *aFig, ptrLead aLead) const {
	( aFig->*myFoo )(aLead);
}

////////////////////////////////////////////////////////////
// Tests
#ifdef GTUT

class cComTestFig: public cFigment, private tOutline<cComTestFig>{
public:
	static const cPlugTag* xPT_testTag;

	static const char* identify(){ return "command test figment"; }

	cComTestFig(){}
	~cComTestFig(){}
};

const cPlugTag* cComTestFig::xPT_testTag = tOutline<cComTestFig>::makePlugTag("test tag");

GTUT_START(command, nothing)
{

}
GTUT_END;

#endif
