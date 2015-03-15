#include "stage.hpp"

using namespace gt;

const cPlugTag*	cStage::xPT_layout = tOutline<cStage>::makePlugTag("layout");
const cPlugTag*	cStage::xPT_fullscreen = tOutline<cStage>::makePlugTag("fullscreen");

const cCommand::dUID cStage::xLinkCloser = tOutline<cStage>::makeCommand(
	"link closer",
	&cStage::patLinkCloser,
	cStage::xPT_links,
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
	mLayout.get().left = 10;
	mLayout.get().top = 10;
	mLayout.get().right = 110;
	mLayout.get().bottom = 110;
	addUpdRoster(&mCloser);
	addUpdRoster(&mLayout);
}

cStage::~cStage(){
}

cFigment::dMigrationPattern
cStage::getLoadPattern(){
	PROFILE;

	dMigrationPattern pattern;
	dVersionPlugs version1;

	version1.push_back(&mLink);
	version1.push_back(&mCloser);
	version1.push_back(&mLayout);

	pattern.push_back(version1);
	return pattern;
}

void
cStage::getLinks(std::list<ptrFig>* pOutLinks){
	PROFILE;
	ASRT_NOTNULL(pOutLinks);
	cChainLink::getLinks(pOutLinks);
	pOutLinks->push_back(mCloser.get());
}

void
cStage::patLinkCloser(ptrLead aLead){
	aLead->copyPlug(&mCloser, xPT_links);
}

void
cStage::patSetLayout(ptrLead aLead){
	aLead->copyPlug(&mLayout, xPT_layout);
	refreshDim();
}

void
cStage::patGetLayout(ptrLead aLead){
	aLead->setPlug(&mLayout, xPT_layout);
}

void
cStage::patSetFullscreen(ptrLead aLead){
	tPlug<bool> fullscreen(false);
	aLead->copyPlug(&fullscreen, xPT_fullscreen);
	setFullscreen(fullscreen.get());
}
