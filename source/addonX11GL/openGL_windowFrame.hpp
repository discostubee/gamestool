
/*
 * !\file
 * !\note	This code comes from 'The game programmers wiki', so big props to them. http://gpwiki.org/index.php/OpenGL:Tutorials:Setting_up_OpenGL_on_X11
 * !\note	Will have to install: libxxf86vm-dev, libgl1-mesa-dev
 */
#ifndef OPENGL_WINDOWFRAME_HPP
#define OPENGL_WINDOWFRAME_HPP

#include "gt_graphics/windowFrame.hpp"

#include <GL/gl.h>
#include <GL/glx.h>
#include <GL/glu.h>

#include <X11/Xlib.h>	// Xlib.h is the default header that is included and has the core functionality
#include <X11/Xatom.h>	// Xatom.h includes functionality for creating new protocol messages
#include <X11/keysym.h> // keysym.h contains key symbols which we use to resolve what keys that are being pressed

/* the XF86 Video Mode extension allows us to change the displaymode of the server
 * this allows us to set the display to fullscreen and also read videomodes and
 * other information.
 */
#include <X11/extensions/xf86vmode.h>

namespace gt{

class cWindowFrame_X11GL: public cWindowFrame, private tOutline<cWindowFrame_X11GL>{

public:
	static const char* identify(){ return "window frame X11GL"; }
	static dNameHash replaces(){ return getHash<cWindowFrame>(); }

	cWindowFrame_X11GL();
	virtual ~cWindowFrame_X11GL();

	virtual const char* name() const{ return identify(); }		//!< Virtual version of identify.
	virtual dNameHash hash() const{ return tOutline<cWindowFrame_X11GL>::hash(); }
	virtual dNameHash getReplacement() const{ return replaces(); }

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
	XF86VidModeModeInfo     mDesktopMode;
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
