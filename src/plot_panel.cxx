#include "plot_panel.h"
#include <FL/Fl_Preferences.H>
#include <ui_controller.h>
#include <cassert>
#include <klv_view.h>
#include <mgl2/Fl_MathGL.h>

#if defined(WIN32) && !defined(__CYGWIN__)
#include <io.h>
#else
#include <unistd.h>
#endif // WIN32 && !__CYGWIN__

using namespace cvtool;
using namespace uavv;

/**
 Window for plot data
*/

PlotWnd::PlotWnd(int W, int H, const char* l) 
            : Fl_Double_Window(W, H, l)
            , plotView(nullptr)
            , pController(nullptr)
{
    
}

void PlotWnd::SetUIController(UIController* controller)
{
    pController = controller;
}

void PlotWnd::SetPlotView(Fl_MGLView* view)
{
    plotView = view;
}

void PlotWnd::UpdatePlot()
{
    assert(plotView);
    if(!plotView)
        return;

    plotView->update();
}

void PlotWnd::ClearData()
{

}

void PlotWnd::CleanUp()
{
    
}