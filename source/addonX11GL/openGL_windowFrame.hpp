
/*
 * !\file	openGL_windowFrame.hpp
 * !\brief	This is sort of the start file for the X11GL addon.
 * !\note	This code comes from 'The game programmers wiki', so big props to them. http://gpwiki.org/index.php/OpenGL:Tutorials:Setting_up_OpenGL_on_X11
 * !\note	Will have to install: mesa-common-dev
 */
#ifndef OPENGL_WINDOWFRAME_HPP
#define OPENGL_WINDOWFRAME_HPP

//#define GL_GLEXT_PROTOTYPES

#include "gt_graphics/windowFrame.hpp"

#include <GL/gl.h>
#include <GL/glx.h>
#include <GL/glext.h>

#include <X11/Xlib.h>	// Xlib.h is the default header that is included and has the core functionality
#include <X11/Xatom.h>	// Xatom.h includes functionality for creating new protocol messages
#include <X11/keysym.h> // keysym.h contains key symbols which we use to resolve what keys that are being pressed

namespace gt{

class X11GLAddon{
public:
	static const dPlaChar* getAddonName() { return "X11GL"; }
};

class cWindowFrame_X11GL: public cWindowFrame, public tAddonDependent<X11GLAddon>{

public:
	cWindowFrame_X11GL();
	virtual ~cWindowFrame_X11GL();

	GT_IDENTIFY("window gl");
	GT_REPLACES(cWindowFrame);
	virtual dNameHash hash() const{ return getHash<cWindowFrame_X11GL>(); }

	virtual void run(cContext* pCon);

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
	bool					mInternalDimRefresh;	//!< Used to prevent circular refresh.

	virtual void refreshDim();

    void testPattern();	//!< Draw a test pattern.

private:
    bool isDestroyWindowAtom(const ::Atom& pAtom);
};

}

#endif
