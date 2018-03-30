#include <ui_controller.h>
#include <resources.h>
#include <cassert>
#include <FL/Fl_Native_File_Chooser.H>
#include <FL/Fl_Input.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_Slider.H>
#include <FL/fl_ask.H>

using namespace std;
using namespace cvtool;

static Fl_Button *loadVideo=(Fl_Button *)0;
static Fl_Button *firstFrameBtn=(Fl_Button *)0;
static Fl_Button *prevFrameBtn=(Fl_Button *)0;
static Fl_Button *playBtn=(Fl_Button *)0;
static Fl_Button *stopBtn=(Fl_Button *)0;
static Fl_Button *nextFrameBtn=(Fl_Button *)0;
static Fl_Button *lastFrameBtn=(Fl_Button *)0;
static Fl_Button *pauseBtn=(Fl_Button *)0;
static Fl_Button *toggleVideoWnd=(Fl_Button *)0;
static Fl_Slider *playTrackbar=(Fl_Slider *)0;
static Fl_Input  *fileNameEdit=(Fl_Input *)0;

static Fl_Image *image_open() {
    static Fl_Image *image = new Fl_RGB_Image(idata_open, 20, 20, 4, 0);
    return image;
}

static Fl_Image *image_first_b() {
    static Fl_Image *image = new Fl_RGB_Image(idata_first_b, 24, 24, 4, 0);
    return image;
}

static Fl_Image *image_prev_b() {
    static Fl_Image *image = new Fl_RGB_Image(idata_prev_b, 24, 24, 4, 0);
    return image;
}

static Fl_Image *image_play_b() {
    static Fl_Image *image = new Fl_RGB_Image(idata_play_b, 24, 24, 4, 0);
    return image;
}

static Fl_Image *image_stop_b() {
    static Fl_Image *image = new Fl_RGB_Image(idata_stop_b, 24, 24, 4, 0);
    return image;
}

static Fl_Image *image_next_b() {
    static Fl_Image *image = new Fl_RGB_Image(idata_next_b, 24, 24, 4, 0);
    return image;
}

static Fl_Image *image_last_b() {
    static Fl_Image *image = new Fl_RGB_Image(idata_last_b, 24, 24, 4, 0);
    return image;
}

static Fl_Image *image_pause_b() {
    static Fl_Image *image = new Fl_RGB_Image(idata_pause_b, 24, 24, 4, 0);
    return image;
}

static Fl_Image *image_toggle() {
    static Fl_Image *image = new Fl_RGB_Image(idata_toggle, 32, 32, 4, 0);
    return image;
}
  
UIController::UIController() 
    : mainWnd(nullptr)
    , renderWnd(nullptr)
    , renderWndVisible(1)
{

}

UIController::~UIController()
{

}

void UIController::InitUIComponents()
{
    shared_ptr<MainWnd> mainTemp(UIController::makeMainPanel(this));
    mainWnd = std::move(mainTemp);
    
    int W = mainWnd->w(); 
    int H = mainWnd->h();
    
    int X = (Fl::w() - mainWnd->w())/2;
    int Y = MAIN_OFFSET_Y;
    mainWnd->resize(X, Y, W, H);

    shared_ptr<RenderWnd> renderTmp(UIController::makeRenderPanel(this, FRAME_WIDTH, FRAME_HEIGHT, "GLFrame"));
    renderWnd = std::move(renderTmp);

    if ((Fl::w() < renderWnd->w()) || (Fl::h() < renderWnd->h()))
        renderWnd->size(Fl::w(), Fl::h());

    int rW = renderWnd->w(); 
    int rH = renderWnd->h();
    
    int rX = (Fl::w() - renderWnd->w())/2;
    int rY = MAIN_OFFSET_Y + mainWnd->decorated_h() + RENDER_OFFSET_Y;
    renderWnd->resize(rX, rY, rW, rH);
}

void UIController::ShowMainWindow(int argc, char *argv[])
{
    assert(mainWnd);
    assert(renderWnd);

    if(!mainWnd)
    {
        fl_message("ERROR: Main window is not initialized");
        return;
    }

    if(!renderWnd)
    {
        fl_message("ERROR: Video playback window is not initialized");
        return;
    }

    mainWnd->show(argc, argv);
    if(renderWndVisible)
        renderWnd->show();
}

    
/**/ UIController* UIController::CreateInstance()
{
    return new UIController();
}

/*static*/ int UIController::RunApplication(int argc, char *argv[])
{
    unique_ptr<UIController> uiController(UIController::CreateInstance());
    
    Fl::lock();

    uiController->InitUIComponents();
    uiController->ShowMainWindow(argc,argv);
    
    // Let FLTK run
    int res = Fl::run();

    // Clean up
    Fl::unlock();
    //uavv_image_destroy(buf);

    return res;
}

/*static*/ void UIController::SetupSystemSettings()
{
    //Fl::scheme("gleam")
    //Fl::scheme("plastic)
   
    Fl::scheme("gtk+");
    Fl::get_system_colors();
    Fl::visual(FL_RGB);
}


/*static*/ void UIController::OnPickFile(Fl_Widget*, void*) 
{
  // Create native chooser
  Fl_Native_File_Chooser native;
  native.title("Choose video file");
  native.type(Fl_Native_File_Chooser::BROWSE_FILE);
  native.filter(fileFilter);
  native.preset_file(fileNameEdit->value());
  // Show native chooser
  switch ( native.show() ) 
  {
    case -1: 
      fprintf(stderr, "ERROR: %s\n", native.errmsg()); 
      fl_message("ERROR: %s\n", native.errmsg());
      break;	// ERROR
    case  1: 
      fprintf(stderr, "*** CANCEL\n"); 
      fl_beep(); 
      break;		// CANCEL
    default: 								// PICKED FILE
      if ( native.filename() ) 
      {
          fileNameEdit->value(native.filename());
      } 
      else 
      {
	        fileNameEdit->value("");
      }
      break;
  }
}

bool UIController::IsVideoRenderVisible() const
{
    return renderWndVisible;
}

void UIController::UpdateRenderVisibility()
{
    assert(renderWnd);
    if(!renderWnd)
        return;

    if(renderWndVisible)
        renderWnd->show();
    else
        renderWnd->hide();
}

void UIController::ToggleRender()
{
    renderWndVisible = !renderWndVisible;
    toggleVideoWnd->value(IsVideoRenderVisible() ? 1 : 0);
    UpdateRenderVisibility();
}

/*static*/ void UIController::OnToggleRender(Fl_Widget* sender, void* pUserData)
{
    assert(pUserData);
    UIController* controller = static_cast<UIController*>(pUserData); 
    if(!controller)
    {
        return;
    }

    controller->ToggleRender();
}

void UIController::FirstFrameClick()
{
    fl_message("Go to start");
}

/*static*/ void UIController::OnFirstFrameClick(Fl_Widget*, void* pUserData)
{
    assert(pUserData);
    UIController* controller = static_cast<UIController*>(pUserData); 
    if(!controller)
    {
        return;
    }

    controller->FirstFrameClick();
}

void UIController::PreviousFrameClick()
{
    fl_message("Step backward");
}

/*static*/ void UIController::OnPreviousFrameClick(Fl_Widget*, void* pUserData)
{
    assert(pUserData);
    UIController* controller = static_cast<UIController*>(pUserData); 
    if(!controller)
    {
        return;
    }

    controller->PreviousFrameClick();
}

void UIController::PlayClick()
{
    fl_message("Play video");
    playBtn->hide();
    pauseBtn->show();
}

/*static*/ void UIController::OnPlayClick(Fl_Widget*, void* pUserData)
{
    assert(pUserData);
    UIController* controller = static_cast<UIController*>(pUserData); 
    if(!controller)
    {
        return;
    }

    controller->PlayClick();
}

void UIController::PauseClick()
{
    fl_message("Pause video playback");
    playBtn->show();
    pauseBtn->hide();
}

/*static*/ void UIController::OnPauseClick(Fl_Widget*, void* pUserData)
{
    assert(pUserData);
    UIController* controller = static_cast<UIController*>(pUserData); 
    if(!controller)
    {
        return;
    }

    controller->PauseClick();
}

void UIController::StopClick()
{
    fl_message("Stop video playback");
    playBtn->show();
    pauseBtn->hide();
}

/*static*/ void UIController::OnStopClick(Fl_Widget*, void* pUserData)
{
    assert(pUserData);
    UIController* controller = static_cast<UIController*>(pUserData); 
    if(!controller)
    {
        return;
    }

    controller->StopClick();
}

void UIController::NextFrameClick()
{
    fl_message("Step forward");
}

/*static*/ void UIController::OnNextFrameClick(Fl_Widget*, void* pUserData)
{
    assert(pUserData);
    UIController* controller = static_cast<UIController*>(pUserData); 
    if(!controller)
    {
        return;
    }

    controller->NextFrameClick();
}

void UIController::LastFrameClick()
{
    fl_message("Go to end");
}

/*static*/ void UIController::OnLastFrameClick(Fl_Widget*, void* pUserData)
{
    assert(pUserData);
    UIController* controller = static_cast<UIController*>(pUserData); 
    if(!controller)
    {
        return;
    }

    controller->LastFrameClick();
}

/*static*/ MainWnd* UIController::makeMainPanel(UIController* controller) 
{  
    MainWnd* main_panel = nullptr;
    { 
        main_panel = new MainWnd(0, 0, 540, 190, "CVTool");
        main_panel->box(FL_FLAT_BOX);
        main_panel->color(FL_BACKGROUND_COLOR);
        main_panel->selection_color(FL_BACKGROUND_COLOR);
        main_panel->labeltype(FL_NO_LABEL);
        main_panel->labelfont(0);
        main_panel->labelsize(14);
        main_panel->labelcolor(FL_FOREGROUND_COLOR);
        main_panel->callback(UIController::OnCloseMainWnd, (void*)controller);
        main_panel->align(Fl_Align(FL_ALIGN_CENTER));
        main_panel->when(FL_WHEN_RELEASE);
        { 
            fileNameEdit = new Fl_Input(25, 25, 465, 25, "Video file");
            //fileNameEdit->en(0);
            fileNameEdit->tooltip("Video file path");
            fileNameEdit->align(Fl_Align(FL_ALIGN_TOP_LEFT));
        } // Fl_Input* fileNameEdit
        { 
            loadVideo = new Fl_Button(490, 25, 25, 25);
            loadVideo->tooltip("Load video file");
            loadVideo->down_box(FL_DOWN_BOX);
            loadVideo->callback(UIController::OnPickFile);
            loadVideo->image( image_open() );   
            //Fl_File_Icon  *icon = Fl_File_Icon::find(".", Fl_File_Icon::DIRECTORY);
            //icon->label(loadVideo);

        } // Fl_Button* loadVideo
        
        { 
            firstFrameBtn = new Fl_Button(25, 125, 40, 40);
            firstFrameBtn->image( image_first_b() );
            firstFrameBtn->align(Fl_Align(512));
            firstFrameBtn->callback(UIController::OnFirstFrameClick, (void*)controller);
        } // Fl_Button* firstFrameBtn
        { 
            prevFrameBtn = new Fl_Button(80, 125, 40, 40);
            prevFrameBtn->image( image_prev_b() );
            prevFrameBtn->align(Fl_Align(512));
            prevFrameBtn->callback(UIController::OnPreviousFrameClick, (void*)controller);
        } // Fl_Button* prevFrameBtn
        { 
            playBtn = new Fl_Button(135, 125, 40, 40);
            playBtn->image( image_play_b() );
            playBtn->align(Fl_Align(512));
            playBtn->callback(UIController::OnPlayClick, (void*)controller);
        } // Fl_Button* playBtn
        { 
            stopBtn = new Fl_Button(190, 125, 40, 40);
            stopBtn->image( image_stop_b() );
            stopBtn->align(Fl_Align(512));
            stopBtn->callback(UIController::OnStopClick, (void*)controller);
        } // Fl_Button* stopBtn
        { 
            nextFrameBtn = new Fl_Button(245, 125, 40, 40);
            nextFrameBtn->image( image_next_b() );
            nextFrameBtn->align(Fl_Align(512));
            nextFrameBtn->callback(UIController::OnNextFrameClick, (void*)controller);
        } // Fl_Button*nextFrameBtn
        { 
            lastFrameBtn = new Fl_Button(300, 125, 40, 40);
            lastFrameBtn->image( image_last_b() );
            lastFrameBtn->align(Fl_Align(512));
            lastFrameBtn->callback(UIController::OnLastFrameClick, (void*)controller);
        } // Fl_Button* lastFrameBtn
        { 
            pauseBtn = new Fl_Button(135, 125, 40, 40);
            pauseBtn->image( image_pause_b() );
            pauseBtn->hide();
            pauseBtn->callback(UIController::OnPauseClick, (void*)controller);
        } // Fl_Button* pauseBtn

        { 
            playTrackbar = new Fl_Slider(25, 75, 490, 27);
            playTrackbar->type(5);
            playTrackbar->box(FL_FLAT_BOX);
            playTrackbar->maximum(100);
            playTrackbar->step(1);
        } // Fl_Slider* playTrackbar
        
        { 
            toggleVideoWnd = new Fl_Button(475, 125, 40, 40);
            toggleVideoWnd->type(1);
            toggleVideoWnd->down_box(FL_DOWN_BOX);
            toggleVideoWnd->selection_color((Fl_Color)55);
            toggleVideoWnd->image( image_toggle() );
            toggleVideoWnd->align(Fl_Align(512));
            toggleVideoWnd->value(controller->IsVideoRenderVisible() ? 1 : 0);
            toggleVideoWnd->callback(UIController::OnToggleRender, (void*)controller);
        } // Fl_Button* toggleVideoWnd

        main_panel->set_non_modal();
        main_panel->size_range(540, 190, 540, 190);
        main_panel->end();
    } // MainWnd* main_panel

    return main_panel;
}

/*static*/ RenderWnd* UIController::makeRenderPanel(UIController* controller, int W, int H, const char* l)
{
    RenderWnd* w;
    { 
        RenderWnd* o = new RenderWnd(W, H, l);
        w = o; if (w) {/* empty */}
        o->box(FL_FLAT_BOX);
        o->color(FL_BACKGROUND_COLOR);
        o->selection_color(FL_BACKGROUND_COLOR);
        o->labeltype(FL_NO_LABEL);
        o->labelfont(0);
        o->labelsize(14);
        o->labelcolor(FL_FOREGROUND_COLOR);
        o->when(FL_WHEN_RELEASE);
        o->callback(UIController::OnToggleRender, (void*)controller);
        o->set_non_modal();
        o->end();
        o->resizable(o);
    } // RenderWnd* o
    return w;
}

void UIController::ExitApplicaion()
{
    if(renderWnd->shown())
        renderWnd->hide();

    exit(0);
}

/*static*/ void UIController::OnCloseMainWnd(Fl_Widget*, void* pUserData)
{
    assert(pUserData);
    UIController* controller = static_cast<UIController*>(pUserData); 
    if(!controller)
    {
        return;
    }

    controller->ExitApplicaion();
}
