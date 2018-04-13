#ifndef zoom_panel_h
#define zoom_panel_h
#include <FL/Fl.H>

#include <stdio.h>
#include <stdlib.h>
#include <uavv_image.h>
#include <zoom_view.h>

#include <FL/Fl_Double_Window.H>
#include <FL/Fl.H>
#include <data_structures.h>

/**
 Video zoom window
*/

namespace cvtool
{
    class UIController;
    class ZoomView;
    class ZoomWnd : public Fl_Double_Window 
    {
            UIController* pController; 
            ZoomView* zoomView;
        public:
            ZoomWnd(int W, int H, const char* l = 0);
            void UpdateGLFrame(const UAVV_IMAGE buf, float scaledX, float scaledY, int zoomParam);
            void SetUIController(UIController* controller);
            void SetView(ZoomView* view);
            void ZoomChanged(int zoomParam);
            void CleanUp();
    };
}

#endif