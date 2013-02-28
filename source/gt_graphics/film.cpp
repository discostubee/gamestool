#include "film.hpp"

using namespace gt;

const cPlugTag*	cFilm::xPT_content = tOutline<cFilm>::makePlugTag("content");
const cPlugTag*	cFilm::xPT_layout = tOutline<cFilm>::makePlugTag("layout");
const cPlugTag*	cFilm::xPT_rez = tOutline<cFilm>::makePlugTag("rez");

const cCommand::dUID cFilm::xLinkContent = tOutline<cFilm>::makeCommand(
	"link content",
	&cFilm::patSetLink,
	cFilm::xPT_content,
	NULL
);

const cCommand::dUID cFilm::xSetLayout = tOutline<cFilm>::makeCommand(
	"set layout",
	&cFilm::patSetLayout,
	cFilm::xPT_layout,
	NULL
);

const cCommand::dUID cFilm::xGetLayout = tOutline<cFilm>::makeCommand(
	"get layout",
	&cFilm::patGetLayout,
	cFilm::xPT_layout,
	NULL
);

const cCommand::dUID cFilm::xGetRez = tOutline<cFilm>::makeCommand(
	"get rez",
	&cFilm::patGetRez,
	cFilm::xPT_rez,
	NULL
);

cFilm::cFilm(){
}

cFilm::~cFilm(){
}

void
cFilm::getLinks(std::list<ptrFig>* pOutLinks){
	PROFILE;
	ASRT_NOTNULL(pOutLinks);
	pOutLinks->push_back(mLink.get());
}

cFigment::dMigrationPattern
cFilm::getLoadPattern(){
	PROFILE;

	dMigrationPattern pattern;
	dVersionPlugs version1;

	version1.push_back(&mLink);
	version1.push_back(&mLayout);

	pattern.push_back(version1);
	return pattern;
}

void
cFilm::patSetLink(ptrLead aLead){
	aLead->setPlug(&mLink, xPT_content);
}

void
cFilm::patSetLayout(ptrLead aLead){
	aLead->setPlug(&mLayout, xPT_layout);
	updateLayout();
}

void
cFilm::patGetLayout(ptrLead aLead){
	aLead->getPlug(&mLayout, xPT_layout);
}

void
cFilm::patGetRez(ptrLead aLead){
	tDim2D<dUnitPix> rez;
	getRez(rez);
	aLead->setValue(&rez, xPT_rez);
}
