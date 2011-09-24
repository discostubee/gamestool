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
cPolyMesh::patAddVert(cLead *aLead){
	promiseLazy();
	for(cLead::cPileItr itr = aLead->getPiledDItr(currentCon); !itr.atEnd(); ++itr){
		mLazyMesh->mVertexes.push_back(
			itr.getPlug()->getCopy<sVertex>()
		);
	}
}

void
cPolyMesh::patAddPoly(cLead *aLead){
	promiseLazy();
	for(cLead::cPileItr itr = aLead->getPiledDItr(currentCon); !itr.atEnd(); ++itr){
		mLazyMesh->mPolys.push_back(
			itr.getPlug()->getCopy<sPoly>()
		);
	}
}

void
cPolyMesh::patGetMesh(cLead *aLead){

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
