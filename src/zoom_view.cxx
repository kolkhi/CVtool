#include "zoom_view.h"
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <FL/filename.H>
#include <FL/fl_ask.H>
#include <FL/Fl_Shared_Image.H>
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

ZoomView::ZoomView(int X, int Y, int W, int H, const char* l) :    
            Fl_Gl_Window(X, Y, W, H, l), mpFrame(nullptr)
{
    mode(FL_DOUBLE);
    mGlFrame.EnableInterpolation(false);
}

void ZoomView::UpdateGLFrame(const UAVV_IMAGE buf)
{
    // Keep a reference to the frame buffer
    if (!buf) 
        return;
    
    std::lock_guard<std::mutex> lock(imageMutex);
    
    IUAVVInterface::DestroyImageHandle(mpFrame);
    mpFrame = IUAVVInterface::CopyImageHandle(buf);
    
    // Schedule a redraw
    redraw();
}


void ZoomView::draw()
{
    if (!valid())  
      ortho();

    std::lock_guard<std::mutex> lock(imageMutex);
    
    // Update GL frame
    if (mpFrame)
    {
        mGlFrame.copy(mpFrame);
        IUAVVInterface::DestroyImageHandle(mpFrame);
        mpFrame = nullptr;
    }

    /*if (!mGlFrame.isValid())  
    {
        return;
    }*/    

    glClearColor(0.0, 0.0, 0.0, 0.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_BLEND);
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);

    // Fit GL frame to window
    float scale_x = w() / (float)mGlFrame.width();
    float scale_y = h() / (float)mGlFrame.height();
    
    glPushMatrix();
    glScalef(scale_x, scale_y, 1.0);

    //mGlFrame.draw();

    glPopMatrix();
    glDisable(GL_BLEND);
}

//
// End of "$Id: renderer_panel.fl 02 2018-03-29 18:21:11Z gsergia $".
//
