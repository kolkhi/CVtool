#ifndef zoom_view_h
#define zoom_view_h
#include <FL/Fl.H>

#include <stdio.h>
#include <stdlib.h>
#include <uavv_image.h>

#include <FL/Fl_Gl_Window.H>
#include <FL/Fl.H>
#include <FL/gl.h>
#include <mutex>
#include <gl_uavv_image.h>

/**
 Video zoom window
*/

namespace cvtool
{
    class Line2f;
    class Rect2f;
    class UIController; 
    class ZoomView : public Fl_Gl_Window 
    {

    private:
        GLUavvImage mGlFrame;
        UAVV_IMAGE  mpFrame;
        std::mutex  imageMutex; 
        int zoomParam;
        int offsetX;
        int offsetY;
        float mouseScaledX;
        float mouseScaledY;
        bool updateZoomValue;
        UIController* pController;

    protected:
        void draw();
        int handle(int event);
        int calculateOffset(int mousePos, int centerPos);
        bool isOffsetOutOfBoundary(int offset, int boundary);
        int moveOffsetToClosestBoundary(int offset, int boundary);
        void findBestZoomParameterAndUpdate(int mouseX, int mouseY, int centerX, int centerY, int width, int height);
        
    public:
        ZoomView(int X, int Y, int W, int H, const char* l = 0);
        void UpdateGLFrame(const UAVV_IMAGE buf, float scaledX, float scaledY, int zoomVal);
        void UpdateGLFrame(const UAVV_IMAGE buf);
        void MakeZoom(int zoomVal);
        void CleanUp();
        void SetUIController(UIController* controller);
        void ClearGLFrame();
    };
}

#endif