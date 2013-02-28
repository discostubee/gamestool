#include "openGL_film.hpp"

using namespace gt;

cFilm_X11GL::cFilm_X11GL(){
}

cFilm_X11GL::~cFilm_X11GL(){
}

void
cFilm_X11GL::work(cContext* pCon){
	mLink.get()->run(pCon);
}

void
cFilm_X11GL::updateLayout(){
}

void
cFilm_X11GL::getRez(tDim2D<dUnitPix> &aSetMe){
}


