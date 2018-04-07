#include "zoom_panel.h"
#include <FL/Fl_Preferences.H>
#include <uavv_wrapper.h>
#include <ui_controller.h>

#if defined(WIN32) && !defined(__CYGWIN__)
#include <io.h>
#else
#include <unistd.h>
#endif // WIN32 && !__CYGWIN__

using namespace cvtool;
using namespace uavv;

/**
 Window for zoom
*/

ZoomWnd::ZoomWnd(int W, int H, const char* l) :    
            Fl_Double_Window(W, H, l)
{
    
}

void ZoomWnd::UpdateGLFrame(const UAVV_IMAGE buf)
{
    // Keep a reference to the frame buffer

}

void ZoomWnd::SetUIController(UIController* controller)
{
    pController = controller;
}

//
// End of "$Id: zoom_panel.fl 02 2018-03-29 18:21:11Z gsergia $".
//
