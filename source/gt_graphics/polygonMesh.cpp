#include "polygonMesh.hpp"

using namespace gt;

////////////////////////////////////////////////////////////
sVertex&
sVertex::operator+= (const sVertex &aCopyMe){
	ASRT_NOTSELF(&aCopyMe);
	x = aCopyMe.x;
	y = aCopyMe.y;
	z = aCopyMe.z;
	return *this;
}

sLine&
sLine::operator+= (const sLine &aCopyMe){
	ASRT_NOTSELF(&aCopyMe);
	a = aCopyMe.a;
	b = aCopyMe.b;
	return *this;
}

sPoly&
sPoly::operator+= (const sPoly &aCopyMe){
	ASRT_NOTSELF(&aCopyMe);
	a = aCopyMe.a;
	b = aCopyMe.b;
	c = aCopyMe.c;
	return *this;
}

sMesh&
sMesh::operator+= (const sMesh &aCopyMe){
	ASRT_NOTSELF(&aCopyMe);
	return *this;
}



////////////////////////////////////////////////////////////
const cPlugTag* cPolyMesh::xPT_vert = tOutline<cPolyMesh>::makePlugTag("vertex");
const cPlugTag* cPolyMesh::xPT_poly = tOutline<cPolyMesh>::makePlugTag("polygon");

const cCommand::dUID cPolyMesh::xAddToMesh = tOutline<cPolyMesh>::makeCommand(
	"add to mesh", &cPolyMesh::patAddToMesh,
	NULL
);

const cCommand::dUID cPolyMesh::xGetMesh = tOutline<cPolyMesh>::makeCommand(
	"get mesh", &cPolyMesh::patGetMesh,
	xPT_vert,
	xPT_poly,
	NULL
);


cPolyMesh::cPolyMesh(): mLazyMesh(NULL){
}

cPolyMesh::~cPolyMesh(){
	cleanLazy();
}

void
cPolyMesh::patAddToMesh(ptrLead aLead){
	PROFILE;

	promiseLazy();
	//aLead->assignTo(&mLazyMesh->mVertexes, xPT_vert);
	//aLead->assignTo(&mLazyMesh->mPolys, xPT_poly);
}

void
cPolyMesh::patGetMesh(ptrLead aLead){
	PROFILE;

	promiseLazy();
	//aLead->assignFrom(mLazyMesh->mVertexes, xPT_vert);
	//aLead->assignFrom(mLazyMesh->mPolys, xPT_poly);
}

void
cPolyMesh::promiseLazy(){
	if(!mLazyMesh) mLazyMesh = new sMesh;
}

void
cPolyMesh::cleanLazy(){
	delete mLazyMesh; mLazyMesh = NULL;
}

sMesh
cPolyMesh::getCurrentMesh(){
	if(!mLazyMesh)
		throw excep::base_error("no mesh", __FILE__, __LINE__);

	return *mLazyMesh;
}

#ifdef GTUT

GTUT_START(testPolyMesh, tagging){

}GTUT_END;

#endif
