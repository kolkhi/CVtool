#include <thumbnail_widget.h>
#include <FL/fl_draw.H>
#include <FL/Fl_Shared_Image.H>
#include <ui_controller.h>
#include <FL/math.h>
#include <cassert>

using namespace cvtool;
/**
 Window for thumbnail
*/

Thumbnail::Thumbnail(int index, int X,int Y,int W,int H,const char* L) 
    : Fl_Box(X,Y,W,H,L)
    , currentIndex(index) 
{
    align(FL_ALIGN_TOP);
    box(FL_FLAT_BOX);
    color(FL_DARK3);
    labelsize(10);
}

void Thumbnail::draw() 
{
    assert(pController);
    Fl_Box::draw();

    Fl_Shared_Image* shrimg = nullptr;
    std::string name;
    auto res = pController->GetThumbnailsController()->GetThumbnailData(currentIndex, name, shrimg); 
    if(res)
    {
        copy_label(name.c_str());
        copy_tooltip(name.c_str());
        if(shrimg)
        {
            int w = shrimg->w();
            int h = shrimg->h();
            shrimg->draw(x(), y(), w, h, 0, 0); 
            shrimg->release();
        }       
    }
    else
    {
        copy_label("");
        copy_tooltip("");
    }
}

int Thumbnail::GetCurrentIndex()
{
    return currentIndex;
}

void Thumbnail::SetUIController(UIController* controller)
{
    pController = controller;
}

int Thumbnail::handle(int event) 
{
  switch(event) 
  {
    case FL_PUSH:
      redraw();
      return 1;
    case FL_RELEASE:
      {
        redraw();
        do_callback();
      }
      return 1;
    case FL_ENTER:
    {
        fl_cursor(FL_CURSOR_HAND);
        return 1;
    }
    break;
    case FL_LEAVE:
    {
        fl_cursor(FL_CURSOR_DEFAULT);
        return 1;
    }
    default:
      return Fl_Box::handle(event);
  }
}