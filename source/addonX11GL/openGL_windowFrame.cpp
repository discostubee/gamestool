#include "openGL_windowFrame.hpp"

using namespace gt;

void
cWindowFrame_X11GL::setDim(dUnitPix pX, dUnitPix pY, dUnitPix pW, dUnitPix pH){
	if(pW==0 || pH==0)
		return;

	glViewport(0, 0, static_cast<GLdouble>(pW), static_cast<GLdouble>(pH));
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(
		45.0,	// Field of view.
		static_cast<GLdouble>(pW / pH), // aspect ratio.
		0.1, // z near clip.
		100.0 // z far clip.
	);
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
    XF86VidModeModeInfo **modes=NULL;
    int modeNum=0, bestMode=0;

    // get a connection
    mDisplay = XOpenDisplay(0);
    mScreen = DefaultScreen(mDisplay);

//#ifdef DEBUG
#if 0
    {
		char** list=NULL;
		int extCount=0;
		int extension=0;
		int vmMajor=0, vmMinor=0;

		list = XListExtensions(mDisplay, &extCount);
		while(extension < extCount){
			DBUG_LO("xlib extension: "<<list[extension]);
			++extension;
		}
		XFreeExtensionList(list);

		XF86VidModeQueryVersion(mDisplay, &vmMajor, &vmMinor);
		DBUG_LO("XF86 VideoMode extension version " << vmMajor << "." << vmMinor);
	}
#endif

	XF86VidModeGetAllModeLines(mDisplay, mScreen, &modeNum, &modes);

    // get an appropriate visual
    vi = glXChooseVisual(mDisplay, mScreen, attrListDoubleBuff);

    if(vi == NULL){
        vi = glXChooseVisual(mDisplay, mScreen, attrListSingleBuff);
        mDoubleBuffered = False;
    }else{
        mDoubleBuffered = True;
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
    mWinAttr.event_mask = ExposureMask | KeyPressMask | ButtonPressMask | StructureNotifyMask;

    if(mFullscreen){	//- Run checks before attempting fullscreen.
		if(modes==NULL){
			mFullscreen = false;
			DBUG_LO("Unable to set fullscreen because we can't get modes.");
		}

		// look for mode with requested resolution
		for (int i = 0; i < modeNum; i++)
		{
			if (
				modes[i]->hdisplay == mWidth
				&& modes[i]->vdisplay == mHeight
				&& XF86VidModeValidateModeLine(mDisplay, 0, modes[i])==0/*MODE_OK*/
			)
				bestMode = i;
		}
    }

    if (mFullscreen){
    	int dpyWidth, dpyHeight;

    	// save desktop-resolution before switching modes
		mDesktopMode = *modes[0];

        XF86VidModeSwitchToMode(mDisplay, mScreen, modes[bestMode]);
        XF86VidModeSetViewPort(mDisplay, mScreen, 0, 0);
        dpyWidth = modes[bestMode]->hdisplay;
        dpyHeight = modes[bestMode]->vdisplay;
        DBUG_LO("resolution " << dpyWidth << ", " << dpyHeight);

        mWinAttr.override_redirect = True;


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

    }else{	// create a window

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
        DBUG_LO("window created ("<<mWidth<<","<<mHeight<<")");
    }

    glXMakeCurrent(mDisplay, mWindow, mContext);

	if(glXIsDirect(mDisplay, mContext)){ DBUG_LO("DRI enabled"); }else{ DBUG_LO("no DRI available"); }

    setDim(0, 0, mWidth, mHeight);

    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glClearDepth(1.0f);
    glShadeModel(GL_SMOOTH);

    glEnable(GL_CULL_FACE);
	glEnable(GL_TEXTURE_2D);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glDisable(GL_DITHER);

	glDepthFunc(GL_LEQUAL);
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    if(modes) XFree(modes);
    XFlush(mDisplay);
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

	// handle the events in the queue
	for(int run=0; run < xEventsPerRun && XPending(mDisplay) > 0; ++run){
		XNextEvent(mDisplay, &mEvent);
		switch (mEvent.type){
			//case Expose:

/*			case DestroyNotify:
				DBUG_LO("destroy notify.");
				pCon->add(this);
				mClosing.mD->run(pCon);
				pCon->finished(this);
				break;*/

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
					DBUG_LO("destroy message. Calling " << mClosing.mD->name());
					pCon->add(this);
					mClosing.mD->run(pCon);
					pCon->finished(this);
				}
			}break;

			default:
				break;
		}
	}

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    if(mContent.mD->hash()==getHash<cEmptyFig>()){
    	testPattern();
    }else{
		pCon->add(this);
		mContent.mD->run(pCon);
		pCon->finished(this);
    }

	if(mDoubleBuffered) glXSwapBuffers(mDisplay, mWindow); else glFlush();
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


void
cWindowFrame_X11GL::testPattern(){

	//- Shamelessly ripping off http://gpwiki.org/index.php/OpenGL:Tutorials:Setting_up_OpenGL_on_X11
	static GLfloat rotate;
	glPushMatrix();
		glTranslatef(0.0f, 0.0f, -7.0f);
		glRotatef(rotate, 1.0f, 0.5f, 0.25f);
		glScalef(0.5f, 0.5f, 0.5f);
		glBegin(GL_QUADS);
			// top of cube
			glColor3f(0.0f, 1.0f, 0.0f);
			glVertex3f(1.0f, 1.0f, -1.0f);
			glVertex3f(-1.0f, 1.0f, -1.0f);
			glVertex3f(-1.0f, 1.0f, 1.0f);
			glVertex3f(1.0f, 1.0f, 1.0f);
			// bottom of cube
			glColor3f(1.0f, 0.5f, 0.0f);
			glVertex3f(1.0f, -1.0f, 1.0f);
			glVertex3f(-1.0f, -1.0f, 1.0f);
			glVertex3f(-1.0f, -1.0f, -1.0f);
			glVertex3f(1.0f, -1.0f, -1.0f);
			// front of cube
			glColor3f(1.0f, 0.0f, 0.0f);
			glVertex3f(1.0f, 1.0f, 1.0f);
			glVertex3f(-1.0f, 1.0f, 1.0f);
			glVertex3f(-1.0f, -1.0f, 1.0f);
			glVertex3f(1.0f, -1.0f, 1.0f);
			// back of cube
			glColor3f(1.0f, 1.0f, 0.0f);
			glVertex3f(-1.0f, 1.0f, -1.0f);
			glVertex3f(1.0f, 1.0f, -1.0f);
			glVertex3f(1.0f, -1.0f, -1.0f);
			glVertex3f(-1.0f, -1.0f, -1.0f);
			// right side of cube
			glColor3f(1.0f, 0.0f, 1.0f);
			glVertex3f(1.0f, 1.0f, -1.0f);
			glVertex3f(1.0f, 1.0f, 1.0f);
			glVertex3f(1.0f, -1.0f, 1.0f);
			glVertex3f(1.0f, -1.0f, -1.0f);
			// left side of cube
			glColor3f(0.0f, 1.0f, 1.0f);
			glVertex3f(-1.0f, 1.0f, 1.0f);
			glVertex3f(-1.0f, 1.0f, -1.0f);
			glVertex3f(-1.0f, -1.0f, -1.0f);
			glVertex3f(-1.0f, -1.0f, 1.0f);
		glEnd();
	glPopMatrix();
    ++rotate;
}
