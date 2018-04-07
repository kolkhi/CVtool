#ifndef zoom_panel_h
#define zoom_panel_h
#include <FL/Fl.H>

#include <stdio.h>
#include <stdlib.h>
#include <uavv_image.h>

#include <FL/Fl_Double_Window.H>
#include <FL/Fl.H>

/**
 Video zoom window
*/

namespace cvtool
{
    class UIController;
    class ZoomWnd : public Fl_Double_Window 
    {
            UIController* pController; 
        public:
            ZoomWnd(int W, int H, const char* l = 0);
            void UpdateGLFrame(const UAVV_IMAGE buf);
            void SetUIController(UIController* controller);
    };
}

#endif