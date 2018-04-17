#ifndef klv_view_h
#define klv_view_h

#include <stdio.h>
#include <string.h>
#ifdef WIN32
#include <stdlib.h>	// atoi
#endif /*WIN32*/

#include <mutex>
#include <array>
#include <vector>

#include <FL/Fl.H>
#include <FL/fl_draw.H>
#include <FL/Fl_Table_Row.H>
#include <data_structures.h>

namespace cvtool
{
    class KLVTableView : public Fl_Table_Row
    {
    private:
        Fl_Color cell_bgcolor;				// color of cell's bg color
        Fl_Color cell_fgcolor;				// color of cell's fg color
        Fl_Color cell_errcolor;				// color of cell's error color

        std::mutex klvMutex;
        std::vector<KLVItem> klvItems;

        std::array<std::string, 3> klvHeaders;

    protected:
        void draw_cell(TableContext context,  		// table cell drawing
                int R=0, int C=0, int X=0, int Y=0, int W=0, int H=0);
        
    public:
        KLVTableView(int x, int y, int w, int h, const char *l=0);
        ~KLVTableView();

        Fl_Color GetCellFGColor() const;
        Fl_Color GetCellBGColor() const;
        Fl_Color GetCellBGErrorColor() const;
        void SetCellFGColor(Fl_Color val);
        void SetCellBGColor(Fl_Color val);

        void UpdateKLVData(const std::vector<KLVItem>& items);
    };
}

#endif