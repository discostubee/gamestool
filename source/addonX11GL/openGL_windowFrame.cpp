#include "openGL_windowFrame.hpp"

using namespace gt;

void
cWindowFrame_X11GL::setDim(dUnitPix pX, dUnitPix pY, dUnitPix pW, dUnitPix pH){
}

cWindowFrame_X11GL::cWindowFrame_X11GL():
	mX(0), mY(0), mWidth(100), mHeight(100), mDepth(0)
{

	int attrListDoubleBuff[] = {
	    GLX_RGBA, GLX_DOUBLEBUFFER,
	    GLX_RED_SIZE, 4,
	    GLX_GREEN_SIZE, 4,
	    GLX_BLUE_SIZE, 4,
	    GLX_DEPTH_SIZE, 16,
	    None
	};

	int attrListSingleBuff[] = {
	    GLX_RGBA, GLX_RED_SIZE, 4,
	    GLX_GREEN_SIZE, 4,
	    GLX_BLUE_SIZE, 4,
	    GLX_DEPTH_SIZE, 16,
	    None
	};

    XVisualInfo *vi = NULL;
    Colormap cmap;
    XF86VidModeModeInfo **modes;
    int modeNum=0, bestMode=0;

    // get a connection
    mDisplay = XOpenDisplay(0);
    mScreen = DefaultScreen(mDisplay);
    XF86VidModeGetAllModeLines(mDisplay, mScreen, &modeNum, &modes);

    {
    	int vmMajor, vmMinor;
    	XF86VidModeQueryVersion(mDisplay, &vmMajor, &vmMinor);
    	DBUG_LO("XF86 VideoMode extension version " << vmMajor << "." << vmMinor);
    }

    // save desktop-resolution before switching modes
    mDesktopMode = *modes[0];

    // get an appropriate visual
    vi = glXChooseVisual(mDisplay, mScreen, attrListDoubleBuff);

    if(vi == NULL){
        vi = glXChooseVisual(mDisplay, mScreen, attrListSingleBuff);
        mDoubleBuffered = False;
        DBUG_LO("single-buffered rendering will be used, no double-buffering available");
    }else{
        mDoubleBuffered = True;
        DBUG_LO("double-buffered rendering available");
    }

    // create a GLX context
    mContext = glXCreateContext(mDisplay, vi, 0, GL_TRUE);

    // create a color map
    cmap = XCreateColormap(
		mDisplay,
		RootWindow(mDisplay, vi->screen),
		vi->visual,
		AllocNone
    );

    mWinAttr.colormap = cmap;
    mWinAttr.border_pixel = 0;

    if (mFullscreen){
    	int dpyWidth, dpyHeight;

    	// look for mode with requested resolution
		for (int i = 0; i < modeNum; i++)
		{
			if ((modes[i]->hdisplay == mWidth) && (modes[i]->vdisplay == mHeight))
				bestMode = i;
		}

        XF86VidModeSwitchToMode(mDisplay, mScreen, modes[bestMode]);
        XF86VidModeSetViewPort(mDisplay, mScreen, 0, 0);
        dpyWidth = modes[bestMode]->hdisplay;
        dpyHeight = modes[bestMode]->vdisplay;
        DBUG_LO("resolution " << dpyWidth << ", " << dpyHeight);
        XFree(modes);

        mWinAttr.override_redirect = True;
        mWinAttr.event_mask = ExposureMask | KeyPressMask | ButtonPressMask | StructureNotifyMask;

        mWindow = XCreateWindow(
        	mDisplay,
        	RootWindow(mDisplay, vi->screen),
            0, 0,
            dpyWidth, dpyHeight,
            0,
            vi->depth,
            InputOutput,
            vi->visual,
            CWBorderPixel | CWColormap | CWEventMask | CWOverrideRedirect,
            &mWinAttr
        );

        XMapRaised(mDisplay, mWindow);

        XWarpPointer(
        	mDisplay,
        	None,
        	mWindow,
        	0, 0, 0, 0, 0, 0
        );

    }else{	// create a window in window mode

    	mWinAttr.event_mask = ExposureMask | KeyPressMask | ButtonPressMask | StructureNotifyMask;

        mWindow = XCreateWindow(
        	mDisplay,
        	RootWindow(mDisplay, vi->screen),
            0, 0, mWidth, mHeight, 0,
            vi->depth,
            InputOutput,
            vi->visual,
            CWBorderPixel | CWColormap | CWEventMask,
            &mWinAttr
        );

        // only set window title and handle wm_delete_events if in windowed mode
		mDeleteMessage = XInternAtom(mDisplay, "WM_DELETE_WINDOW", True);
		XSetWMProtocols(mDisplay, mWindow, &mDeleteMessage, 1);

        XSetStandardProperties(
        	mDisplay,
        	mWindow,
        	"PORN",
            "FREE PORN",
            None,
            NULL,
            0,
            NULL
        );

        XMapRaised(mDisplay, mWindow);
    }

    setDim(0, 0, mWidth, mHeight);
}

cWindowFrame_X11GL::~cWindowFrame_X11GL(){
	// switch back to original desktop resolution if we were in fullscreen
	if(mFullscreen){
		XF86VidModeSwitchToMode(mDisplay, mScreen, &mDesktopMode);
		XF86VidModeSetViewPort(mDisplay, mScreen, 0, 0);
	}

	if(mContext){
		if( !glXMakeCurrent(mDisplay, None, NULL)){
			DBUG_LO("Could not release drawing context.");
		}

		glXDestroyContext(mDisplay, mContext);
		mContext = NULL;
	}

	XCloseDisplay(mDisplay);
}

void
cWindowFrame_X11GL::run(cContext* pCon){
	bool reestablish = false;

	//!!! temp
	{
		static bool test = true;

		if(test)
			reestablish = true;

		test = false;
	}

    // Test to see if the window frame has changed.
	if(reestablish){
#ifdef DEBUG
		int glxMajor=0, glxMinor=0;
		glXQueryVersion(mDisplay, &glxMajor, &glxMinor);
		DBUG_LO("GLX-Version " << glxMajor << "." << glxMinor);
#endif

		// connect the glx-context to the window
		glXMakeCurrent(mDisplay, mWindow, mContext);

		if( glXIsDirect(mDisplay, mContext) ){
			DBUG_LO("DRI enabled");
		}else{
			DBUG_LO("no DRI available");
		}

		reestablish = false;
	}

	// handle the events in the queue
	for(int run=0; run < xEventsPerRun; ++run){
		XNextEvent(mDisplay, &mEvent);
		switch (mEvent.type){
			//case Expose:

			case DestroyNotify:
				DBUG_LO("destroy notify.");
				pCon->add(this);
				mContent.mD->run(pCon);
				pCon->finished(this);
				break;

			case ConfigureNotify:
				if (
					static_cast<unsigned int>(mEvent.xconfigure.width) != mWidth ||
					static_cast<unsigned int>(mEvent.xconfigure.height) != mHeight
				){
					setDim(0, 0, mEvent.xconfigure.width, mEvent.xconfigure.height);
				}
				break;

			case ButtonPress:
				break;

			case KeyPress:
				if (XLookupKeysym(&mEvent.xkey, 0) == XK_Escape){
				}
				break;

			case ClientMessage:{
				DBUG_LO("client message " << XGetAtomName( mDisplay, mEvent.xclient.message_type ));
				if( isDestroyWindowAtom(mEvent.xclient.message_type) ){
					DBUG_LO("destroy message. Calling " << mContent.mD->name());
					pCon->add(this);
					mContent.mD->run(pCon);
					pCon->finished(this);
				}
			}break;

			default:
				break;
		}
	}

	pCon->add(this);
	mContent.mD->run(pCon);
	pCon->finished(this);

	glFlush();
}


bool
cWindowFrame_X11GL::isDestroyWindowAtom(const ::Atom& pAtom){
	static const char* DestroyWindowAtomName = "WM_PROTOCOLS";
	static const size_t AtomNameLen = strlen(DestroyWindowAtomName);

	if(
		::strncmp(
			::XGetAtomName( mDisplay, mEvent.xclient.message_type ),
			DestroyWindowAtomName, AtomNameLen
		) == 0
	)
		return true;

	return false;
}
