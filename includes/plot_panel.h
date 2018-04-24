#ifndef plot_panel_h
#define plot_panel_h
#include <FL/Fl.H>

#include <vector>
#include <FL/Fl_Double_Window.H>
#include <FL/Fl.H>
#include <data_structures.h>
#include <mgl2/Fl_MathGL.h>

/**
 Video klv window
*/

namespace cvtool
{
    class UIController;
    class PlotWnd : public Fl_Double_Window 
    {
            Fl_MGLView* plotView;
            UIController* pController;

        public:
            PlotWnd(int W, int H, const char* l = 0);
            void SetUIController(UIController* controller);
            void SetPlotView(Fl_MGLView* view);
            void UpdatePlot();
            void ClearData();
            void CleanUp();
    };
}

#endif