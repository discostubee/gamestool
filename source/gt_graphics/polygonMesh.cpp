#include "polygonMesh.hpp"

using namespace gt;

const cPlugTag* cPolyMesh::xPT_Vert = tOutline<cPolyMesh>::makePlugTag("vertex");
const cPlugTag* cPolyMesh::xPT_Poly = tOutline<cPolyMesh>::makePlugTag("polygon");
const cPlugTag* cPolyMesh::xPT_Mesh = tOutline<cPolyMesh>::makePlugTag("mesh");
const cCommand* cPolyMesh::xAddVert = tOutline<cPolyMesh>::makeCommand("add vertex", cPolyMesh::eAddVert, cPolyMesh::xPT_Vert);
const cCommand* cPolyMesh::xAddPoly = tOutline<cPolyMesh>::makeCommand("add polygon", cPolyMesh::eAddPoly, cPolyMesh::xPT_Poly);
const cCommand* cPolyMesh::xGetMesh = tOutline<cPolyMesh>::makeCommand("get mesh", cPolyMesh::eGetMesh, cPolyMesh::xPT_Mesh);


cPolyMesh::cPolyMesh(): mLazyMesh(NULL){
}

cPolyMesh::~cPolyMesh(){
	cleanLazy();
}

void
cPolyMesh::jack(ptrLead pLead, cContext *pCon){
	PROFILE;

	try{
		switch( pLead->mCom->getSwitch<cPolyMesh>() ){
			case eAddVert:{
				PROFILE;
				promiseLazy();
				mLazyMesh->mVertexes.push_back(
					pLead->getPlug(cPolyMesh::xPT_Vert, pCon)->getCopy<sVertex>()
				);
			}break;

			case eAddPoly:{
				PROFILE;
				promiseLazy();
				mLazyMesh->mPolys.push_back(
					pLead->getPlug(cPolyMesh::xPT_Poly, pCon)->getCopy<sPoly>()
				);
			}break;

			default:
			case eNotMyBag:{ DBUG_LO("not my bag"); }break;
		}
	}catch(excep::base_error &e){
		WARN(e);
	}catch(...){
		UNKNOWN_ERROR;
	}

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
	DONT_USE_THIS; return sMesh();
}
