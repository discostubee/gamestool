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
	mVertexes.insert(mVertexes.end(), aCopyMe.mVertexes.begin(), aCopyMe.mVertexes.end());
	mPolys.insert(mPolys.end(), aCopyMe.mPolys.begin(), aCopyMe.mPolys.end());
	mTMap.insert(mTMap.end(), aCopyMe.mTMap.begin(), aCopyMe.mTMap.end());
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

	tPlugLinearContainer<sVertex, std::vector> vecs;
	tPlugLinearContainer<sPoly, std::vector> polies;
	aLead->copyPlug(&vecs, xPT_vertexs);
	aLead->copyPlug(&polies, xPT_polies);
	for(
		tPlugLinearContainer<sVertex, std::vector>::dItr itr = vecs.getItr();
		itr.stillGood();
		++itr
	)
		mLazyMesh.get().mVertexes.push_back( itr.get().get() );

	for(
		tPlugLinearContainer<sPoly, std::vector>::dItr itr = polies.getItr();
		itr.stillGood();
		++itr
	)
		mLazyMesh.get().mPolys.push_back( itr.get().get() );

	mLazyMesh.get().mFresh = true;
}

void
cPolyMesh::patGetMesh(ptrLead aLead){
	PROFILE;

	downloadLazy();
	tPlugLinearContainer<sVertex, std::vector> vecs(
		mLazyMesh.get().mVertexes
	);
	tPlugLinearContainer<sPoly, std::vector> polies(
		mLazyMesh.get().mPolys
	);
	aLead->setPlug(&vecs, xPT_vertexs);
	aLead->setPlug(&polies, xPT_polies);
}

void
cPolyMesh::patMeasure(ptrLead aLead){
	PROFILE;
	if(aLead->hasTag(xPT_box)){
		tPlug< geometry::tCube<dUnitVDis> > cube;
		measure(cube.get());
		aLead->setPlug(&cube, xPT_box);
	}else if(aLead->hasTag(xPT_sphere)){
		tPlug< geometry::tSphere<dUnitVDis> > sphere;
		measure(sphere.get());
		aLead->setPlug(&sphere, xPT_sphere);
	}
}

void
cPolyMesh::patTexturize(ptrLead aLead){
	PROFILE;
	tPlugLinearContainer<sTexMap, std::vector> texs;
	aLead->copyPlug(&texs, xPT_texMapping);
	for(
		tPlugLinearContainer<sTexMap, std::vector>::dItr itr = texs.getItr();
		itr.stillGood();
		++itr
	)
		mLazyMesh.get().mTMap.push_back( itr.get().get() );

	mLazyMesh.get().mFresh = true;
}

void
cPolyMesh::cleanLazy(){
	mLazyMesh.get().mPolys.clear();
	mLazyMesh.get().mTMap.clear();
	mLazyMesh.get().mVertexes.clear();
}


