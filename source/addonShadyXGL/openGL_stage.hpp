
/*
 * !\file	openGL_stage.hpp
 * !\note	This code comes from 'The game programmers wiki', so big props to them. http://gpwiki.org/index.php/OpenGL:Tutorials:Setting_up_OpenGL_on_X11
 * !\note	Will have to install: mesa-common-dev
 */
#ifndef OPENGL_STAGE_HPP
#define OPENGL_STAGE_HPP

#ifndef GL_GLEXT_PROTOTYPES
#	define GL_GLEXT_PROTOTYPES
#endif

#include "addonShadyXGL.hpp"
#include "gt_graphics/stage.hpp"

#include <GL/gl.h>
#include <GL/glx.h>
#include <GL/glext.h>

#include <X11/Xlib.h>	// Xlib.h is the default header that is included and has the core functionality
#include <X11/Xatom.h>	// Xatom.h includes functionality for creating new protocol messages
#include <X11/keysym.h> // keysym.h contains key symbols which we use to resolve what keys that are being pressed

namespace gt{

class cStage_X11GL: public cStage{
public:
	cStage_X11GL();
	virtual ~cStage_X11GL();

	GT_IDENTIFY("stage gl");
	GT_REPLACES(cStage);
	virtual dNameHash hash() const{ return getHash<cStage_X11GL>(); }

	virtual void work(cContext* pCon);

protected:
	static const unsigned short xEventsPerRun = 3;

	Display               * mDisplay;
	int                     mScreen;
	Window                  mWindow;
	GLXContext              mContext;
	XSetWindowAttributes    mWinAttr;
	Bool                    mFullscreen;
	Bool                    mDoubleBuffered;
	unsigned int            mDepth;
    XEvent					mEvent;
	Atom 					mDeleteMessage;
	bool					mInternalRefresh;	//!< Used to prevent circular refresh.

	virtual void refreshDim();
	virtual void setFullscreen(bool pFullescreen);

    void testPattern();	//!< Draw a test pattern.

private:
    bool isDestroyWindowAtom(const ::Atom& pAtom);
};

}

#endif
