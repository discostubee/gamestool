#include "openGL_windowFrame.hpp"



////////////////////////////////////////////////////////////
using namespace win;

LRESULT CALLBACK 
win::WndProc(
	HWND	windowsHandle, 
	UINT	message, 
	WPARAM	firstMessageParameter, 
	LPARAM	secondMessageParameter
){
	switch (message){
		case WM_CREATE:
			break;
	        
		case WM_CLOSE:
		case WM_DESTROY:	
			gt::cWindowFrame_winGL::getFigFromWinHand(windowsHandle)->close();
			break;

		default:
			return ::DefWindowProc(windowsHandle, message, firstMessageParameter, secondMessageParameter);
			break;
	}
	return(0);
}

////////////////////////////////////////////////////////////
using namespace gt;

// Statics
cWindowFrame_winGL::dMapWinFig cWindowFrame_winGL::mMapWinToFig;

// Methos
cWindowFrame_winGL::cWindowFrame_winGL():
	mCloseMe(false)
{
	PROFILE;

	//- sets up a style for the window
	mWinClass.style = CS_OWNDC;
	mWinClass.lpfnWndProc = win::WndProc;
	mWinClass.cbClsExtra = 0;
	mWinClass.cbWndExtra = 0;
	mWinClass.hInstance = win::getInst();
	mWinClass.hIcon = LoadIcon (NULL, IDI_APPLICATION);
	mWinClass.hCursor = LoadCursor(NULL, IDC_ARROW);
	mWinClass.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
	mWinClass.lpszMenuName = NULL;
	mWinClass.lpszClassName = TEXT("testStyle");

	if(! ::RegisterClass(&mWinClass)){		
		DBUG_LO("Can't register classes");	// this needs to be more.

	}else{
		//- sets up the pixel format
		::ZeroMemory (&mPFD, sizeof (mPFD));
		mPFD.nSize = sizeof (mPFD);
		mPFD.nVersion = 1;
		mPFD.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
		mPFD.iPixelType = PFD_TYPE_RGBA;
		mPFD.cColorBits = 24;
		mPFD.cDepthBits = 16;
		mPFD.cStencilBits = 8;
		mPFD.iLayerType = PFD_MAIN_PLANE;

		//- create the window
		mWinHandle = ::CreateWindow(
			mWinClass.lpszClassName,		
			TEXT("un-named program"),
			WS_CAPTION | WS_POPUPWINDOW | WS_VISIBLE,	// options
			mX.mD, mX.mD,								// position in windows
			mHeight.mD, mWidth.mD,						// dimensions
			NULL, 
			NULL, 
			win::getInst(), 
			NULL
		);

		//- registration
		mDevCon = ::GetDC(mWinHandle);	// get the device context from the window's handle.
		::SetPixelFormat(				// sets up the pixel format for the window.
			mDevCon, 
			::ChoosePixelFormat(
				mDevCon, 
				&mPFD
			), 
			&mPFD
		);
		mRenderCon	= ::wglCreateContext( mDevCon ); 

		mMapWinToFig[mWinHandle] = this;
		
		::wglMakeCurrent(mDevCon, mRenderCon);	// temp code to get textures working.
	}
}

cWindowFrame_winGL::~cWindowFrame_winGL(){
	PROFILE;

	::wglMakeCurrent(NULL, NULL);
    ::wglDeleteContext(mRenderCon);
	::ReleaseDC(mWinHandle, mDevCon);
	::DestroyWindow(mWinHandle);
	mMapWinToFig.erase(mWinHandle);
}



void 
cWindowFrame_winGL::run(cContext* pCon){
	PROFILE;

	ASRT_NOTNULL(pCon);

	wglMakeCurrent(mDevCon, mRenderCon);

	if (::PeekMessage (&mMsg, NULL, 0, 0, PM_REMOVE)){
        ::TranslateMessage (&mMsg);
        ::DispatchMessage (&mMsg);
    }

	pCon->add(this);
	if(mCloseMe)
		mClosing.mD->run(pCon);
	else
		mContent.mD->run(pCon);
	pCon->finished(this);

	SwapBuffers(mDevCon);
}

cWindowFrame_winGL*
cWindowFrame_winGL::getFigFromWinHand(HWND pHandle)
{
	dMapWinFig::iterator itr = mMapWinToFig.find(pHandle);
	
	if(itr == mMapWinToFig.end())
		throw excep::notFound("window handle", __FILE__, __LINE__);

	return itr->second;
}

void 
cWindowFrame_winGL::refreshDim(){
}

void 
cWindowFrame_winGL::close(){
	mCloseMe = true;
}
