#include "stage.hpp"

using namespace gt;

const cPlugTag*	cStage::xPT_content = tOutline<cStage>::makePlugTag("content");
const cPlugTag*	cStage::xPT_closer = tOutline<cStage>::makePlugTag("closer");
const cPlugTag*	cStage::xPT_layout = tOutline<cStage>::makePlugTag("layout");
const cPlugTag*	cStage::xPT_fullscreen = tOutline<cStage>::makePlugTag("fullscreen");

const cCommand::dUID cStage::xLinkContent= tOutline<cStage>::makeCommand(
	"link content",
	&cStage::patLinkContent,
	cStage::xPT_content,
	NULL
);

const cCommand::dUID cStage::xLinkCloser= tOutline<cStage>::makeCommand(
	"link closer",
	&cStage::patLinkCloser,
	cStage::xPT_closer,
	NULL
);

const cCommand::dUID cStage::xSetLayout= tOutline<cStage>::makeCommand(
	"set layout",
	&cStage::patSetLayout,
	cStage::xPT_layout,
	NULL
);

const cCommand::dUID cStage::xGetLayout= tOutline<cStage>::makeCommand(
	"get layout",
	&cStage::patGetLayout,
	cStage::xPT_layout,
	NULL
);

const cCommand::dUID cStage::xSetFullscreen= tOutline<cStage>::makeCommand(
	"set fullscreen",
	&cStage::patSetFullscreen,
	cStage::xPT_fullscreen,
	NULL
);

cStage::cStage(){
}

cStage::~cStage(){
}

cFigment::dMigrationPattern
cStage::getLoadPattern(){
	PROFILE;

	dMigrationPattern pattern;
	dVersionPlugs version1;

	version1.push_back(&mContent);
	version1.push_back(&mCloser);
	version1.push_back(&mLayout);

	pattern.push_back(version1);
	return pattern;
}

void
cStage::getLinks(std::list<ptrFig>* pOutLinks){
	PROFILE;
	ASRT_NOTNULL(pOutLinks);
	pOutLinks->push_back(mContent.get());
	pOutLinks->push_back(mCloser.get());
}

void
cStage::patLinkContent(ptrLead aLead){
	aLead->getPlug(&mContent, xPT_content);
}

void
cStage::patLinkCloser(ptrLead aLead){
	aLead->getPlug(&mCloser, xPT_closer);
}

void
cStage::patSetLayout(ptrLead aLead){
	aLead->getPlug(&mLayout, xPT_layout);
	refreshDim();
}

void
cStage::patGetLayout(ptrLead aLead){
	aLead->setPlug(&mLayout, xPT_layout);
}

void
cStage::patSetFullscreen(ptrLead aLead){
	bool fullscreen=false;
	aLead->getValue(&fullscreen, xPT_fullscreen);
	setFullscreen(fullscreen);
}
