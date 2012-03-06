#include "command.hpp"
#include "figment.hpp"

////////////////////////////////////////////////////////////
using namespace gt;

const cCommand::dUID cCommand::noID = static_cast<cCommand::dUID>(-1);

cCommand::cCommand(
	const dUID pID,
	const char* pName,
	const dNameHash pParentHash
):
	mID(pID), mName(pName), mParent(pParentHash)
{
	DBUG_TRACK_START("command");
}

cCommand::~cCommand(){
	DBUG_TRACK_END("command");
}

////////////////////////////////////////////////////////////
// Tests
#ifdef GTUT

class cComTestFig: public cFigment, private tOutline<cComTestFig>{
public:
	static const cPlugTag* xPT_testTag;

	static const dNatChar* identify(){ return "command test figment"; }

	cComTestFig(){}
	~cComTestFig(){}
};

const cPlugTag* cComTestFig::xPT_testTag = tOutline<cComTestFig>::makePlugTag("test tag");

GTUT_START(command, nothing){

}
GTUT_END;

#endif
