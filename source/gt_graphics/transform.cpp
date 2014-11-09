#include "transform.hpp"

using namespace gt;

const cPlugTag*	cTransform::xPT_pos = tOutline<cTransform>::makePlugTag("pos");
const cPlugTag*	cTransform::xPT_rot = tOutline<cTransform>::makePlugTag("rot");
const cPlugTag*	cTransform::xPT_sca = tOutline<cTransform>::makePlugTag("sca");

const cCommand::dUID cTransform::xGetTransforms = tOutline<cTransform>::makeCommand(
	"get transforms",
	&cTransform::patGetTransforms,
	cTransform::xPT_pos,
	cTransform::xPT_rot,
	cTransform::xPT_sca,
	NULL
);

const cCommand::dUID cTransform::xSetTransforms = tOutline<cTransform>::makeCommand(
	"set transforms",
	&cTransform::patSetTransforms,
	cTransform::xPT_pos,
	cTransform::xPT_rot,
	cTransform::xPT_sca,
	NULL
);

cTransform::cTransform(){
}

cTransform::~cTransform(){
}

cFigment::dMigrationPattern
cTransform::getLoadPattern(){
	PROFILE;

	dMigrationPattern pattern;
	dVersionPlugs version1;

	version1.push_back(&mPos);
	version1.push_back(&mRot);
	version1.push_back(&mSca);

	pattern.push_back(version1);
	return pattern;
}

void cTransform::patGetTransforms(ptrLead aLead){
	getTransforms();
	aLead->setPlug(&mPos, xPT_pos);
	aLead->setPlug(&mRot, xPT_rot);
	aLead->setPlug(&mSca, xPT_sca);
}

void cTransform::patSetTransforms(ptrLead aLead){
	aLead->copyPlug(&mPos, xPT_pos);
	aLead->copyPlug(&mRot, xPT_rot);
	aLead->copyPlug(&mSca, xPT_sca);
	setTransforms();
}

