#include "zoom_view.h"
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <cmath>
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

ZoomView::ZoomView(int X, int Y, int W, int H, const char* l)
            : Fl_Gl_Window(X, Y, W, H, l) 
            , mpFrame(nullptr)
            , zoomParam(1)
            , mouseScaledX(0.0)
            , mouseScaledY(0.0)
            
{
    mode(FL_DOUBLE);
    mGlFrame.EnableInterpolation(false);
}

void ZoomView::UpdateGLFrame(const UAVV_IMAGE buf, float scaledX, float scaledY, int zoomVal)
{
    // Keep a reference to the frame buffer
    if (!buf) 
        return;
    
    std::lock_guard<std::mutex> lock(imageMutex);
    
    IUAVVInterface::DestroyImageHandle(mpFrame);
    mpFrame = IUAVVInterface::CopyImageHandle(buf);
    
    mouseScaledX = scaledX;
    mouseScaledY = scaledY;

    zoomParam = zoomVal;

    // Schedule a redraw
    redraw();
}


void ZoomView::draw()
{
    if (!valid())  
    {
        ortho();
    } 

    std::lock_guard<std::mutex> lock(imageMutex);
    
    // Update GL frame
    if (mpFrame)
    {
        mGlFrame.copy(mpFrame);
    }  

    glClearColor(0.0, 0.0, 0.0, 0.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_BLEND);
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);

    // Fit GL frame to window
    float scale_x = w() / (float)mGlFrame.width();
    float scale_y = h() / (float)mGlFrame.height();
    
    if (mGlFrame.isValid())  
    {
        int centerX = w()/2 / scale_x;
        int centerY = h()/2 / scale_y;
        int winWidth = w();
        int winHeight = h();
        int width = mGlFrame.width();
        int height = mGlFrame.height();
        int mouseX = std::ceil(mouseScaledX * (float)width);
        int mouseY = std::ceil(mouseScaledY * (float)height);
        
        
        glPushMatrix();
        glScalef(scale_x, scale_y, 0);
        //glTranslatef(-centerX * zoomParam, -centerY * zoomParam, 0.0);  
        glScalef(zoomParam, zoomParam, 0.0);

        int offsetX = (mouseX - std::ceil(mouseX/zoomParam));
        int offsetY = (mouseY - std::ceil(mouseY/zoomParam));
        int maxOffsetX = (width - std::ceil(width/zoomParam));
        int maxOffsetY = (height - std::ceil(height/zoomParam));
        
        glTranslatef(-offsetX , -offsetY , 0.0);
        //glTranslatef(centerX / zoomParam , centerY / zoomParam , 0.0);        
        
        mGlFrame.draw();

        glPopMatrix();
        glDisable(GL_BLEND);
    } 
}

void ZoomView::MakeZoom(int zoomVal)
{
    zoomParam = zoomVal;
    redraw();
}

void ZoomView::CleanUp()
{
    std::lock_guard<std::mutex> lock(imageMutex);
    IUAVVInterface::DestroyImageHandle(mpFrame);
    mpFrame = nullptr;
}

//
// End of "$Id: renderer_panel.fl 02 2018-03-29 18:21:11Z gsergia $".
//
