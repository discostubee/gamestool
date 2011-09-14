#include "polygonMesh.hpp"

using namespace gt;

const cPlugTag* cPolyMesh::xPT_Mesh = tOutline<cPolyMesh>::makePlugTag("mesh");
const cCommand* cPolyMesh::xAddVert = tOutline<cPolyMesh>::makeCommand("add vertex", cPolyMesh::eAddVert);
const cCommand* cPolyMesh::xAddPoly = tOutline<cPolyMesh>::makeCommand("add polygon", cPolyMesh::eAddPoly);
const cCommand* cPolyMesh::xGetMesh = tOutline<cPolyMesh>::makeCommand("get mesh", cPolyMesh::eGetMesh);


cPolyMesh::cPolyMesh(): mLazyMesh(NULL){
}

cPolyMesh::~cPolyMesh(){
	cleanLazy();
}

void
cPolyMesh::jack(ptrLead pLead, cContext *pCon){
	PROFILE;

	start(pCon);
	try{
		switch( pLead->mCom->getSwitch<cPolyMesh>() ){
			case eAddVert:{
				PROFILE;
				promiseLazy();
				for(cLead::cPileItr itr = pLead->getPiledDItr(pCon); !itr.atEnd(); ++itr){
					mLazyMesh->mVertexes.push_back(
						itr.getPlug()->getCopy<sVertex>()
					);
				}
			}break;

			case eAddPoly:{
				PROFILE;
				promiseLazy();
				for(cLead::cPileItr itr = pLead->getPiledDItr(pCon); !itr.atEnd(); ++itr){
					mLazyMesh->mPolys.push_back(
						itr.getPlug()->getCopy<sPoly>()
					);
				}
			}break;

			case eGetMesh:{

			}break;

			default:
				cFigment::jack(pLead, pCon);
			break;
		}
	}catch(excep::base_error &e){
		WARN(e);
	}catch(...){
		UNKNOWN_ERROR;
	}
	stop(pCon);
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
