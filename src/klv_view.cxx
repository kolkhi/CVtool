#include <klv_view.h>
#include <stdio.h>
#include <string>
#ifdef WIN32
#include <stdlib.h>	// atoi
#endif /*WIN32*/

#include <FL/Fl.H>
#include <FL/fl_draw.H>

using namespace cvtool;

KLVTableView::KLVTableView(int x, int y, int w, int h, const char *l /*=0*/) 
    : Fl_Table_Row(x,y,w,h,l)
{
    klvHeaders[0] = "Name"; 
    klvHeaders[1] = "Value";
    klvHeaders[2] = "State";
    cell_bgcolor = FL_WHITE;
    cell_fgcolor = FL_BLACK;
    cell_errcolor =  0xff8e8e00;
    end();
}

KLVTableView::~KLVTableView() 
{ 

}

// Handle drawing all cells in table
void KLVTableView::draw_cell(TableContext context, 
			  int R, int C, int X, int Y, int W, int H)
{
    std::lock_guard<std::mutex> lock(klvMutex);
    switch ( context )
    {
	case CONTEXT_STARTPAGE:
	    fl_font(FL_HELVETICA, 14);
	    return;

	case CONTEXT_COL_HEADER:
	    fl_push_clip(X, Y, W, H);
	    {
            std::string s = klvHeaders[C];
            fl_draw_box(FL_THIN_UP_BOX, X, Y, W, H, col_header_color());
            fl_color(FL_BLACK);
            fl_draw(s.c_str(), X, Y, W, H, FL_ALIGN_CENTER);
	    }
	    fl_pop_clip();
	    return;

	case CONTEXT_CELL:
	{
	    fl_push_clip(X, Y, W, H);
	    {
            std::string s;
            Fl_Align align = FL_ALIGN_LEFT;
            Fl_Color backColor = cell_bgcolor;
            if(R < (int)klvItems.size())
            {

                switch(C)
                {
                    case 0:
                        s = klvItems[R].itemName;
                        align = FL_ALIGN_LEFT;
                        break;
                    case 1:
                        s = klvItems[R].itemValue;
                        align = FL_ALIGN_LEFT;
                        break;
                    case 2:
                        s = klvItems[R].GetStateString();
                        align = FL_ALIGN_CENTER;
                        if(klvItems[R].itemState)
                            backColor = cell_errcolor;
                        break;
                    default:
                        break;
                }
            }

	        // BG COLOR
            fl_color( row_selected(R) ? selection_color() : backColor);
            fl_rectf(X, Y, W, H);

            // TEXT
            fl_color(cell_fgcolor);
            fl_draw(s.c_str(), X, Y, W, H, align);

            // BORDER
            fl_color(color()); 
            fl_rect(X, Y, W, H);
	    }
	    fl_pop_clip();
	    return;
	}

	case CONTEXT_TABLE:
	    fprintf(stderr, "TABLE CONTEXT CALLED\n");
	    return;

    case CONTEXT_ROW_HEADER:
	case CONTEXT_ENDPAGE:
	case CONTEXT_RC_RESIZE:
	case CONTEXT_NONE:
	    return;
    }
}

Fl_Color KLVTableView::GetCellFGColor() const 
{ 
    return(cell_fgcolor); 
}

Fl_Color KLVTableView::GetCellBGColor() const 
{ 
    return(cell_bgcolor); 
}

void KLVTableView::SetCellFGColor(Fl_Color val) 
{ 
    cell_fgcolor = val; 
}

void KLVTableView::SetCellBGColor(Fl_Color val) 
{ 
    cell_bgcolor = val; 
}

void KLVTableView::UpdateKLVData(const std::vector<KLVItem>& items)
{
    std::lock_guard<std::mutex> lock(klvMutex);
    klvItems.clear();
    klvItems.insert(klvItems.begin(), items.begin(), items.end());
}