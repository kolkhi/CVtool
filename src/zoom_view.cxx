#include "zoom_view.h"
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <cmath>
#include <cassert>
#include <FL/filename.H>
#include <FL/fl_ask.H>
#include <FL/Fl_Shared_Image.H>
#include <FL/Fl_Preferences.H>
#include <uavv_wrapper.h>
#include <ui_controller.h>
#include <draw_model.h>

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
            , offsetX(0)
            , offsetY(0)
            , mouseScaledX(0.0)
            , mouseScaledY(0.0)
            , updateZoomValue(false)
            , pController(nullptr)
            
{
    mode(FL_DOUBLE);
    mGlFrame.EnableInterpolation(false);
}

void ZoomView::ClearGLFrame()
{
    IUAVVInterface::DestroyImageHandle(mpFrame);
    mpFrame = nullptr;

    pController->GetDrawController()->ClearDrawing();
    redraw();
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

    // Schedule zoom value update on new image add 
    updateZoomValue = true;

    // Schedule a redraw
    redraw();
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

//#define DEBUG_GL_ZOOM
void ZoomView::draw()
{
    if (!valid())  
    {
        ortho();
    } 

    std::lock_guard<std::mutex> lock(imageMutex);
    
    // Update GL frame
    mGlFrame.copy(mpFrame);
    
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
         // scaled center point 
        int centerX = w()/2 / scale_x;
        int centerY = h()/2 / scale_y;

        // image width/height
        int width = mGlFrame.width();
        int height = mGlFrame.height();

        // mouse click relative position, converted to absolute position in current window coordinates 
        int mouseX = std::ceil(mouseScaledX * (float)width);
        int mouseY = std::ceil(mouseScaledY * (float)height);

        // calculate offset from mouse click to center point
        offsetX = calculateOffset(mouseX, centerX);
        offsetY = calculateOffset(mouseY, centerY);

        if(updateZoomValue && (isOffsetOutOfBoundary(offsetX, width) || isOffsetOutOfBoundary(offsetY, height)))
        {
            findBestZoomParameterAndUpdate(mouseX, mouseY, centerX, centerY, width, height);
            offsetX = calculateOffset(mouseX, centerX);
            offsetY = calculateOffset(mouseY, centerY);
        }

        if(isOffsetOutOfBoundary(offsetX, width))
            offsetX = moveOffsetToClosestBoundary(offsetX, width);

        if(isOffsetOutOfBoundary(offsetY, height))
            offsetY = moveOffsetToClosestBoundary(offsetY, height);

        updateZoomValue = false;
            
        glPushMatrix();

        // scale image to fit to current window
        glScalef(scale_x, scale_y, 0);

        // scale image with zooming scale
        glScalef(zoomParam, zoomParam, 0.0);

        // make offset of the clicked point toward center point of the window
        glTranslatef(-offsetX, -offsetY, 0.0);
        
        // draw GL frame
        mGlFrame.draw();


        
 #ifdef DEBUG_GL_ZOOM
        // draw red point at the center position 
        glColor3f(1.0,0.0,0.0);
        
        glBegin(GL_POLYGON);
        glVertex2f(centerX-2, centerY-2);
        glVertex2f(centerX+2, centerY-2);
        glVertex2f(centerX+2, centerY+2);
        glVertex2f(centerX-2, centerY+2);
        glEnd();

        // draw green point at the click position 
        glColor3f(0.0,1.0,0.0);
        
        glBegin(GL_POLYGON);
        glVertex2f(mouseX-2, mouseY-2);
        glVertex2f(mouseX+2, mouseY-2);
        glVertex2f(mouseX+2, mouseY+2);
        glVertex2f(mouseX-2, mouseY+2);
        glEnd();
#endif

        pController->GetDrawController()->drawGeometry();

        glPopMatrix();
        glDisable(GL_BLEND);
    } 
}

int ZoomView::calculateOffset(int mousePos, int centerPos)
{
    // calculate offset component from mouse click to center point
    int offset = (mousePos - std::ceil((float)centerPos/(float)zoomParam));
    return offset;
}

bool ZoomView::isOffsetOutOfBoundary(int offset, int boundary)
{
    // calclate maximum offset of the image from the top and right boundaries (minimum offset from left and bottom boundaries is 0 )
    int minOffset = 0;
    int maxOffset = (boundary - std::ceil((float)boundary/(float)zoomParam));
        
    if(offset < minOffset || offset > maxOffset)
        return true;

    return false;
}

int ZoomView::moveOffsetToClosestBoundary(int offset, int boundary)
{
    // calclate maximum offset of the image from the top and right boundaries (minimum offset from left and bottom boundaries is 0 )
    int minOffset = 0;
    int maxOffset = (boundary - std::ceil((float)boundary/(float)zoomParam));
        
    if(offset < minOffset)
        offset = minOffset;

    if(offset > maxOffset)
        offset = maxOffset;

    return offset;
}

void ZoomView::findBestZoomParameterAndUpdate(int mouseX, int mouseY, int centerX, int centerY, int width, int height)
{
    assert(pController);
    if(!pController)
        return;
    
    ZoomValue currentZoomValue = pController->GetCurrentZoomValue();
    int newZoomValue = static_cast<int>(currentZoomValue);

    // start searching from next value after current 
    for(auto i=static_cast<int>(currentZoomValue) + 1; i < static_cast<int>(ZoomValue::ZoomValueLast); i++)
    {
        newZoomValue = i;
        zoomParam = UIController::GetZoomScaleByValue(static_cast<ZoomValue>(newZoomValue));

        int localOffsetX = calculateOffset(mouseX, centerX);
        int localOffsetY = calculateOffset(mouseY, centerY);
        if( !isOffsetOutOfBoundary(localOffsetX, width) && !isOffsetOutOfBoundary(localOffsetY, height) )
        {
            break;
        }
    }

    pController->UpdateCurrentZoomValue(static_cast<ZoomValue>(newZoomValue));
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

void ZoomView::SetUIController(UIController* controller)
{
    pController = controller;
}

int ZoomView::handle(int event) 
{
    auto convertCoordinates = [&](float& x, float& y)
    {
        if(mGlFrame.width() == 0 || mGlFrame.height() == 0)
            return;

        float scale_x = (float)w() / (float)mGlFrame.width();
        float scale_y = (float)h() / (float)mGlFrame.height();
        
        x /= scale_x;
        y /= scale_y;

        x /= zoomParam;
        y /= zoomParam;

        x += offsetX;
        y += offsetY;
    };

    switch(event) 
    {
    case FL_PUSH:
        {
            //... mouse down event ...
            //... position in Fl::event_x() and Fl::event_y()
            {
                if (!valid())
                {
                    ortho();
                    valid(1);
                }

                float xpos_scaled =  (float)Fl::event_x();// / (float)w();
                float ypos_scaled =  (float)(h() - Fl::event_y());// / (float)h();

                convertCoordinates(xpos_scaled, ypos_scaled);
               
                if(Fl::event_button() == FL_LEFT_MOUSE)
                    pController->OnZoomMouseLeftDown(xpos_scaled, ypos_scaled);
                else if(Fl::event_button() == FL_RIGHT_MOUSE)
                    pController->OnZoomMouseRightDown(xpos_scaled, ypos_scaled);
            }
        }
        return 1;
    case FL_RELEASE:   
        //... mouse up event ...
            {
                if (!valid())
                {
                    ortho();
                    valid(1);
                }

                float xpos_scaled =  (float)Fl::event_x();// / (float)w();
                float ypos_scaled =  (float)(h() - Fl::event_y());// / (float)h();
                
                convertCoordinates(xpos_scaled, ypos_scaled);

                if(Fl::event_button() == FL_LEFT_MOUSE)
                    pController->OnZoomMouseLeftUp(xpos_scaled, ypos_scaled);
                else if(Fl::event_button() == FL_RIGHT_MOUSE)
                    pController->OnZoomMouseRightUp(xpos_scaled, ypos_scaled);
            }
        return 1;
    case FL_MOVE:
        //... mouse moved event ...
            {
                if (!valid())
                {
                    ortho();
                    valid(1);
                }

                float xpos_scaled =  (float)Fl::event_x();// / (float)w();
                float ypos_scaled =  (float)(h() - Fl::event_y());// / (float)h();

                convertCoordinates(xpos_scaled, ypos_scaled);

                pController->OnZoomMouseMove(xpos_scaled, ypos_scaled);
            }
        return 1;
    /*
    case FL_FOCUS :
    case FL_UNFOCUS :
        ... Return 1 if you want keyboard events, 0 otherwise
        return 1;
    case FL_KEYBOARD:
        ... keypress, key is in Fl::event_key(), ascii in Fl::event_text()
        ... Return 1 if you understand/use the keyboard event, 0 otherwise...
        return 1;
    case FL_SHORTCUT:
        ... shortcut, key is in Fl::event_key(), ascii in Fl::event_text()
        ... Return 1 if you understand/use the shortcut event, 0 otherwise...
        return 1;
    */
    default:
        // pass other events to the base class...
        return Fl_Gl_Window::handle(event);
    }
}