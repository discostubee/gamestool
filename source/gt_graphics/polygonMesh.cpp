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

sTexMap&
sTexMap::operator+= (const sTexMap &aCopyMe){
	ASRT_NOTSELF(&aCopyMe);
	memcpy(u, aCopyMe.u, sizeof(u));
	memcpy(v, aCopyMe.v, sizeof(v));
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
//	mVertexes.;
//	mPolys.;
//	mTMap.;
	mFresh = true;
	return *this;
}


////////////////////////////////////////////////////////////
const cPlugTag* cPolyMesh::xPT_vertexs = tOutline<cPolyMesh>::makePlugTag("vertexes");
const cPlugTag* cPolyMesh::xPT_polies = tOutline<cPolyMesh>::makePlugTag("polies");
const cPlugTag*	cPolyMesh::xPT_box = tOutline<cPolyMesh>::makePlugTag("box");
const cPlugTag*	cPolyMesh::xPT_sphere = tOutline<cPolyMesh>::makePlugTag("sphere");
const cPlugTag*	cPolyMesh::xPT_bitmaps = tOutline<cPolyMesh>::makePlugTag("bitmaps");
const cPlugTag*	cPolyMesh::xPT_texMapping = tOutline<cPolyMesh>::makePlugTag("texture mapping");

const cCommand::dUID cPolyMesh::xAddToMesh = tOutline<cPolyMesh>::makeCommand(
	"add to mesh", &cPolyMesh::patAddToMesh,
	cPolyMesh::xPT_vertexs,
	cPolyMesh::xPT_polies,
	NULL
);

const cCommand::dUID cPolyMesh::xGetMesh = tOutline<cPolyMesh>::makeCommand(
	"get mesh", &cPolyMesh::patGetMesh,
	cPolyMesh::xPT_vertexs,
	cPolyMesh::xPT_polies,
	NULL
);

const cCommand::dUID cPolyMesh::xMeasure = tOutline<cPolyMesh>::makeCommand(
	"measure", &cPolyMesh::patMeasure,
	cPolyMesh::xPT_box,
	cPolyMesh::xPT_sphere,
	NULL
);

const cCommand::dUID cPolyMesh::xTexturize = tOutline<cPolyMesh>::makeCommand(
	"texturize", &cPolyMesh::patTexturize,
	cPolyMesh::xPT_bitmaps,
	cPolyMesh::xPT_texMapping,
	NULL
);


cPolyMesh::cPolyMesh(){
	mLazyMesh.get().mFresh = false;
	addUpdRoster(&mLazyMesh);
}

cPolyMesh::~cPolyMesh(){
	cleanLazy();
}

cFigment::dMigrationPattern
cPolyMesh::getLoadPattern(){
	PROFILE;

	dMigrationPattern pattern;
	dVersionPlugs version1;

	downloadLazy();
	version1.push_back(&mLazyMesh);
	version1.push_back(&mBMaps);

	pattern.push_back(version1);
	return pattern;
}


void
cPolyMesh::patAddToMesh(ptrLead aLead){
	PROFILE;

	aLead->appendTo(&mLazyMesh.get().mVertexes, xPT_vertexs);
	aLead->appendTo(&mLazyMesh.get().mPolys, xPT_polies);
	mLazyMesh.get().mFresh = true;
}

void
cPolyMesh::patGetMesh(ptrLead aLead){
	PROFILE;

	downloadLazy();
	aLead->appendFrom(mLazyMesh.get().mVertexes, xPT_vertexs);
	aLead->appendFrom(mLazyMesh.get().mPolys, xPT_polies);
	aLead->appendFrom(mLazyMesh.get().mPolys, xPT_texMapping);
}

void
cPolyMesh::patMeasure(ptrLead aLead){
	PROFILE;
	if(aLead->has(xPT_box)){
		geometry::tCube<dUnitVDis> cube;
		measure(cube);
		aLead->assignFrom(cube, xPT_box);
	}else if(aLead->has(xPT_sphere)){
		geometry::tSphere<dUnitVDis> sphere;
		measure(sphere);
		aLead->assignFrom(sphere, xPT_sphere);
	}
}

void
cPolyMesh::patTexturize(ptrLead aLead){
	PROFILE;

	aLead->appendTo(&mLazyMesh.get().mTMap, xPT_texMapping);
	aLead->plugAppends(&mBMaps, xPT_bitmaps);
	mLazyMesh.get().mFresh = true;
}

void
cPolyMesh::cleanLazy(){
	mLazyMesh.get().mPolys.clear();
	mLazyMesh.get().mTMap.clear();
	mLazyMesh.get().mVertexes.clear();
}


#ifdef GTUT

GTUT_START(testPolyMesh, tagging){

}GTUT_END;

#endif
