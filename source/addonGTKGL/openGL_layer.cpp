#include "openGL_layer.hpp"

using namespace gt;

Gtk::Main cLayer_gtkGL::gGTKMain(0, NULL);

cLayer_gtkGL::cLayer_gtkGL(){
	//!\todo make this thread safe.
	Gtk::Main::iteration(false);
}

cLayer_gtkGL::~cLayer_gtkGL(){
}

void
cLayer_gtkGL::run(cContext* pCon){
	mRoot.mD->run(pCon);
}
