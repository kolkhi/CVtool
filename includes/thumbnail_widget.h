#ifndef thumbnail_widget_h
#define thumbnail_widget_h

#include <FL/Fl.H>
#include <FL/Fl_Box.H>

namespace cvtool
{
    class UIController; 
    class Thumbnail : public Fl_Box 
    {
        UIController* pController;
        int currentIndex;

        void draw();
        int handle(int event);
    public:
        Thumbnail(int index, int X,int Y,int W,int H,const char* L);
        
        int GetCurrentIndex();
        void SetUIController(UIController* controller);
    };
}

#endif