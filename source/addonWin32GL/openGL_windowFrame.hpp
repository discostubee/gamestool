
#ifndef OPENGL_WINDOWFRAME_HPP
#define OPENGL_WINDOWFRAME_HPP

#include <gt_graphics/windowFrame.hpp>
#include <gt_win7/win_world.hpp>

#include <windows.h>
#include <GL/gl.h>
#include <GL/glu.h>

namespace win{

	//!\brief	
	LRESULT CALLBACK WndProc(
		HWND	windowsHandle, 
		UINT	message, 
		WPARAM	firstMessageParameter, 
		LPARAM	secondMessageParameter
	);
}

namespace gt{

	class cWinGLAddon{
	public:
		static const dPlaChar* getAddonName() { return "win32GL"; }
	};

	class cWindowFrame_winGL: public cWindowFrame, public tAddonDependant<cWinGLAddon>{
	public:
		cWindowFrame_winGL();
		virtual ~cWindowFrame_winGL();

		GT_IDENTIFY("window gl");
		GT_REPLACES(cWindowFrame);

		virtual dNameHash hash const(){ return getHash<cWindowFrame_winGL>(); }
		virtual void run(cContext* pCon);

	protected:
		static cWindowFrame_winGL* getFigFromWinHand(HWND pHandle);

		virtual void refreshDim();

		void close();	//!< Once you call this, the next time the frame is run it will caller the closer.

		friend LRESULT CALLBACK win::WndProc(HWND, UINT, WPARAM, LPARAM);	// so it can get the mapped handles when processing messages.

	private:
		::PIXELFORMATDESCRIPTOR		mPFD;			//!< ...
		::WNDCLASS					mWinClass;		//!< window class type. May not need to set this up every time.
		::HDC						mDevCon;		//!< device context
		::HGLRC						mRenderCon;		//!< render context
		::HWND						mWinHandle;		//!< stores the reference to the new window
		::MSG 						mMsg;
		bool						mCloseMe;		//!< Used to cause the window to close next time it is run.

		typedef std::map<::HWND, cWindowFrame_winGL*> dMapWinFig;
		static dMapWinFig mMapWinToFig;	//!< It should be fine to use a static because these classes only exist in the DLL.

	};

}

#endif
