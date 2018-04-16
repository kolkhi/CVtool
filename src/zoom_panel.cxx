#include "zoom_panel.h"
#include <FL/Fl_Preferences.H>
#include <uavv_wrapper.h>
#include <ui_controller.h>
#include <cassert>

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

ZoomWnd::ZoomWnd(int W, int H, const char* l) 
            : Fl_Double_Window(W, H, l)
            , pController(nullptr)
            , zoomView(nullptr)
{
    
}

void ZoomWnd::UpdateGLFrame(const UAVV_IMAGE buf, float scaledX, float scaledY, int zoomParam)
{
    assert(zoomView);
    if(!zoomView)
        return;

    zoomView->UpdateGLFrame(buf, scaledX, scaledY, zoomParam);
}

void ZoomWnd::ClearGLFrame()
{
    assert(zoomView);
    if(!zoomView)
        return;

    zoomView->ClearGLFrame();
}

void ZoomWnd::SetUIController(UIController* controller)
{
    pController = controller;
}

void ZoomWnd::SetView(ZoomView* view)
{
    zoomView = view;
    zoomView->user_data(static_cast<void*>(this));
}

void ZoomWnd::ZoomChanged(int zoomParam)
{
    assert(zoomView);
    if(!zoomView)
        return;

    zoomView->MakeZoom(zoomParam);
}

void ZoomWnd::CleanUp()
{
    assert(zoomView);
    if(!zoomView)
        return;

    zoomView->CleanUp();
}


//
// End of "$Id: zoom_panel.fl 02 2018-03-29 18:21:11Z gsergia $".
//
