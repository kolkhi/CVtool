#include <FL/Fl.H>
//#include <FL/Fl_File_Icon.H>
#include <FL/Fl_Shared_Image.H>
#include <main_panel.h>
#include <render_panel.h>

using namespace cvtool;

int main(int  argc,	char *argv[])
{
    /*Fl::scheme("gtk+");
    choice->add("none");
  choice->add("gtk+");
  choice->add("gleam");
  choice->add("plastic");*/

    Fl::lock();

    Fl::scheme("gtk+");
    Fl::get_system_colors();
    Fl::visual(FL_RGB);

    //Fl_File_Icon::load_system_icons();
    
    main_panel = MainWnd::make_main_panel();

    render_panel = RenderWnd::make_video_panel(FRAME_WIDTH, FRAME_HEIGHT, "GLFrame");
    if ((Fl::w() < render_panel->w()) || (Fl::h() < render_panel->h()))
        render_panel->size(Fl::w(), Fl::h());

    main_panel->show(argc,argv);
    render_panel->show();

    // Let FLTK run
    int res = Fl::run();

    // Clean up
    Fl::unlock();
    //uavv_image_destroy(buf);

    delete render_panel;
    delete main_panel;

    return res;
}

