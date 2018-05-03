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

int KLVTableView::handle(int event) 
{
    switch(event) 
    {
    case FL_FOCUS :
    case FL_UNFOCUS :
        //... Return 1 if you want keyboard events, 0 otherwise
        return 1;
    case FL_KEYBOARD:
        //... keypress, key is in Fl::event_key(), ascii in Fl::event_text()
        //... Return 1 if you understand/use the keyboard event, 0 otherwise...
        return 0;
    case FL_SHORTCUT:
        //... shortcut, key is in Fl::event_key(), ascii in Fl::event_text()
        //... Return 1 if you understand/use the shortcut event, 0 otherwise...
        {
            int k = Fl::event_key();
            int state = Fl::event_state(FL_CTRL);
            
            // Ctrl+c || Ctrl+C
            if((k == 67 || k == 99) && state)
            {
                CopySelectedDataToClipboard();
                return 1;
            }

            // Ctrl+a || Ctrl+A
            if((k == 65 || k == 97) && state)
            {
                select_all_rows();
                return 1;
            }
        }
        return 0;
    default:
        // pass other events to the base class...
        return Fl_Table_Row::handle(event);
    }
}

void KLVTableView::CopySelectedDataToClipboard()
{
    std::string copyDataString;
    for(int i=0; i<rows(); i++)
    {
        if(i >= static_cast<int>(klvItems.size()))
            break;

        int selected = row_selected(i);
        if(selected == 1)
        {
            copyDataString += klvItems[i].itemName + "\t" + klvItems[i].itemValue + "\t" + klvItems[i].GetStateString() + "\n"; 
        }
    }

    Fl::copy(copyDataString.c_str(), static_cast<int>(copyDataString.size()), 1);
}