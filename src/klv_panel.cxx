#include "klv_panel.h"
#include <FL/Fl_Preferences.H>
#include <uavv_wrapper.h>
#include <ui_controller.h>
#include <cassert>
#include <klv_view.h>

#if defined(WIN32) && !defined(__CYGWIN__)
#include <io.h>
#else
#include <unistd.h>
#endif // WIN32 && !__CYGWIN__

using namespace cvtool;
using namespace uavv;

/**
 Window for klv data
*/

KLVWnd::KLVWnd(int W, int H, const char* l) 
            : Fl_Double_Window(W, H, l)
            , klvView(nullptr)
            , pController(nullptr)
{
    
}

void KLVWnd::SetUIController(UIController* controller)
{
    pController = controller;
}

void KLVWnd::SetKLVDataView(KLVTableView* view)
{
    klvView = view;
}

void KLVWnd::UpdateKLVData(const std::vector<KLVItem>& klvItems)
{
    assert(klvView);
    if(!klvView)
        return;

    klvView->UpdateKLVData(klvItems);

    redraw();
}

void KLVWnd::CleanUp()
{
    
}