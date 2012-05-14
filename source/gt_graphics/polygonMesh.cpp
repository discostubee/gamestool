#include "polygonMesh.hpp"

using namespace gt;

const cPlugTag* cPolyMesh::xPT_Mesh = tOutline<cPolyMesh>::makePlugTag("mesh");

const cCommand::dUID cPolyMesh::xAddVert = tOutline<cPolyMesh>::makeCommand(
	"add vertex", &cPolyMesh::patAddVert,
	NULL
);

const cCommand::dUID cPolyMesh::xAddPoly = tOutline<cPolyMesh>::makeCommand(
	"add polygon", &cPolyMesh::patAddPoly,
	NULL
);

const cCommand::dUID cPolyMesh::xGetMesh = tOutline<cPolyMesh>::makeCommand(
	"get mesh", &cPolyMesh::patGetMesh,
	NULL
);


cPolyMesh::cPolyMesh(): mLazyMesh(NULL){
}

cPolyMesh::~cPolyMesh(){
	cleanLazy();
}

void
cPolyMesh::patAddVert(ptrLead aLead){
	PROFILE;

	promiseLazy();
	std::vector< tPlug<sVertex> > verts;
	aLead->getPile(&verts);
	for(std::vector< tPlug<sVertex> >::iterator itr = verts.begin(); itr != verts.end(); ++itr){
		mLazyMesh->mVertexes.push_back( itr->mD );
	}
}

void
cPolyMesh::patAddPoly(ptrLead aLead){
	PROFILE;

	promiseLazy();
	std::vector< tPlug<sPoly> > polys;
	aLead->getPile(&polys);
	for(std::vector< tPlug<sPoly> >::iterator itr = polys.begin(); itr != polys.end(); ++itr){
		mLazyMesh->mPolys.push_back( itr->mD );
	}
}

void
cPolyMesh::patGetMesh(ptrLead aLead){

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
