#include "film.hpp"

using namespace gt;

const cPlugTag*	cFilm::xPT_layout = tOutline<cFilm>::makePlugTag("layout");
const cPlugTag*	cFilm::xPT_rez = tOutline<cFilm>::makePlugTag("rez");

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
cFilm::patSetLayout(ptrLead aLead){
	aLead->setPlug(&mLayout, xPT_layout);
	updateLayout();
}

void
cFilm::patGetLayout(ptrLead aLead){
	aLead->setPlug(&mLayout, xPT_layout);
}

void
cFilm::patGetRez(ptrLead aLead){
	tPlug< tDim2D<dUnitPix> > rez;
	getRez(rez.get());
	aLead->setPlug(&rez, xPT_rez);
}
