#include "openGL_stage.hpp"

using namespace gt;

cStage_X11GL::cStage_X11GL()
: mDepth(0), mInternalRefresh(true)
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

    // get a connection
    mDisplay = XOpenDisplay(0);
    mScreen = DefaultScreen(mDisplay);

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

	mWindow = XCreateWindow(
		mDisplay,
		RootWindow(mDisplay, vi->screen),
		0, 0,
		100, 100,
		0,
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
	DBUG_LO("window created. openGL version: " << GL_VERSION);

	XFlush(mDisplay);

    glXMakeCurrent(mDisplay, mWindow, mContext);

	if(glXIsDirect(mDisplay, mContext)){ DBUG_LO("DRI enabled"); }else{ DBUG_LO("no DRI available"); }

    refreshDim();
}

cStage_X11GL::~cStage_X11GL(){
	try{
		// switch back to original desktop resolution if we were in fullscreen
		if(mFullscreen){

		}

		if(mContext){
			if( !glXMakeCurrent(mDisplay, None, NULL)){
				DBUG_LO("Could not release drawing context.");
			}

			glXDestroyContext(mDisplay, mContext);
			mContext = NULL;
		}

		XCloseDisplay(mDisplay);
	}catch(std::exception &e){

	}catch(...){

	}
}

void
cStage_X11GL::work(cContext* pCon){
	PROFILE;

	// handle the events in the queue
	for(int run=0; run < xEventsPerRun && XPending(mDisplay) > 0; ++run){
		XNextEvent(mDisplay, &mEvent);
		switch (mEvent.type){
			case Expose:
				break;

/*			case DestroyNotify:
				DBUG_LO("destroy notify.");
				pCon->add(this);
				mClosing.mD->run(pCon);
				pCon->finished(this);
				break;*/

			case ConfigureNotify:
				mInternalRefresh = false;
				mLayout.get().right = mLayout.get().left + static_cast<dUnitPix>(mEvent.xconfigure.width);
				mLayout.get().bottom = mLayout.get().top + static_cast<dUnitPix>(mEvent.xconfigure.height);
				refreshDim();
				//DBUG_LO("configure notify "<<mWidth.mD<<","<<mHeight.mD);
				mInternalRefresh = true;
				break;

			case ButtonPress:
				break;

			case KeyPress:
				if (XLookupKeysym(&mEvent.xkey, 0) == XK_Escape){
				}
				break;

			case ClientMessage:{
				DBUG_LO("client message " << XGetAtomName( mDisplay, mEvent.xclient.message_type ));
				if( isDestroyWindowAtom(mEvent.xclient.message_type) && mCloser.get().valid()){
					DBUG_LO("destroy message. Calling " << mCloser.get()->name());
					mCloser.get()->run(pCon);
				}
			}break;

			default:
				break;
		}
	}


    glEnable(GL_CULL_FACE);
	glEnable(GL_TEXTURE_2D);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glDisable(GL_DITHER);

	glEnableClientState(GL_VERTEX_ARRAY);

	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClearDepth(1.0f);
	glShadeModel(GL_SMOOTH);

	glDepthFunc(GL_LEQUAL);
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	if(!mLink.get().valid() || mLink.get()->hash()==getHash<cEmptyFig>()){
		testPattern();
	}else{
		mLink.get()->run(pCon);
	}

	if(mDoubleBuffered)
		glXSwapBuffers(mDisplay, mWindow);
	else
		glFlush();
}


bool
cStage_X11GL::isDestroyWindowAtom(const ::Atom& pAtom){
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
cStage_X11GL::refreshDim(){
	GLdouble glW = static_cast<GLdouble>(mLayout.get().right - mLayout.get().left);
	GLdouble glH = static_cast<GLdouble>(mLayout.get().bottom - mLayout.get().top);

	if(glW == 0)
		glW = 1;

	if(glH == 0)
		glH = 1;

	glViewport(0, 0, glW, glH);

	if(mInternalRefresh){
/*
		if (mFullscreen){
			//!\todo make this configurable.

			DBUG_LO("resolution " << lgW << ", " << glH);

			mWinAttr.override_redirect = True;

			mWindow = XCreateWindow(
				mDisplay,
				RootWindow(mDisplay, vi->screen),
				0, 0,
				glW, glH,
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
		}
*/

		XMoveResizeWindow(
			mDisplay, mWindow,
			static_cast<int>(mLayout.get().left),
			static_cast<int>(mLayout.get().bottom),
			static_cast<unsigned int>(glW),
			static_cast<unsigned int>(glH)
		);
	}
}

void
cStage_X11GL::setFullscreen(bool pFullescreen){

}


void
cStage_X11GL::testPattern(){

	//- Shamelessly ripping off http://gpwiki.org/index.php/OpenGL:Tutorials:Setting_up_OpenGL_on_X11
	static GLfloat rotate;
	glPushMatrix();
		glTranslatef(0.0f, 0.0f, 0.0f);
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
