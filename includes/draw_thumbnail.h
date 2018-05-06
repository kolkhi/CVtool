#ifndef draw_thumbnail_h
#define draw_thumbnail_h

#include <FL/Fl.H>
#include <FL/Fl_Box.H>
#include <FL/Fl_Gl_Window.H>
#include <FL/Fl.H>
#include <FL/gl.h>
#include <mutex>
#include <gl_uavv_image.h>

namespace cvtool
{
    class UIController; 
    class Thumbnail : public Fl_Box 
    {
        UIController* pController;
        int currentIndex;

        GLUavvImage mGlFrame;
        std::mutex  imageMutex;

        void draw();
        int handle(int event);
    public:
        Thumbnail(int index, int X,int Y,int W,int H,const char* L);
        
        int GetCurrentIndex();
        void SetUIController(UIController* controller);
    };
}

#endif