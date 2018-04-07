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
    class ZoomView : public Fl_Gl_Window 
    {

    private:
        GLUavvImage mGlFrame;
        UAVV_IMAGE  mpFrame;
        std::mutex  imageMutex; 
        int zoomParam;
        float mouseScaledX;
        float mouseScaledY;

    protected:
        void draw();

    public:
        ZoomView(int X, int Y, int W, int H, const char* l = 0);
        void UpdateGLFrame(const UAVV_IMAGE buf, float scaledX, float scaledY, int zoomVal);
        void MakeZoom(int zoomVal);
        void CleanUp();
    };
}

#endif