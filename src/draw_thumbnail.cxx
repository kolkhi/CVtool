#include <draw_thumbnail.h>
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
    fl_cursor(FL_CURSOR_HAND);
}

void Thumbnail::draw() 
{
    assert(pController);
    Fl_Box::draw();

    auto thumbData = pController->GetThumbnailData(currentIndex); 
    if(thumbData)
    {
        copy_label(thumbData->imgName.c_str());
        copy_tooltip(thumbData->imgName.c_str());
        Fl_RGB_Image* img = pController->ConvertImage(thumbData->img);
        if(img)
        {
            float iw = (float)w() / (float)img->w();
            float ih = (float)h() / (float)img->h();
            
            Fl_Shared_Image *shrimg = Fl_Shared_Image::get(img);
            if (img->w() > w() || img->h() > h()) 
            {
                Fl_Image *temp;
                temp = img->copy(img->w() * iw, img->h() * ih);
                shrimg->release();
                shrimg = (Fl_Shared_Image*) temp;
            }
            int w = shrimg->w();
            int h = shrimg->h();
            shrimg->draw(x(), y(), w, h, 0, 0);
            shrimg->release();
        }       
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
    default:
      return Fl_Widget::handle(event);
  }
}