#ifndef klv_panel_h
#define klv_panel_h
#include <FL/Fl.H>

#include <vector>
#include <FL/Fl_Double_Window.H>
#include <FL/Fl.H>
#include <data_structures.h>

/**
 Video klv window
*/

namespace cvtool
{
    class KLVTableView;
    class UIController;
    class KLVWnd : public Fl_Double_Window 
    {
            KLVTableView* klvView;
            UIController* pController;
        
        public:
            KLVWnd(int W, int H, const char* l = 0);
            void SetUIController(UIController* controller);
            void SetKLVDataView(KLVTableView* view);
            void UpdateKLVData(const std::vector<KLVItem>& items);
            void ClearData();
            void CleanUp();  
    };
}

#endif