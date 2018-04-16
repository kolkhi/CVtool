#include <ui_controller.h>
#include <resources.h>
#include <cassert>
#include <FL/Fl_Native_File_Chooser.H>
#include <FL/Fl_Input.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_Slider.H>
#include <Fl/Fl_Value_Slider.H>
#include <FL/fl_ask.H>
#include <Fl/Fl_Box.H>
#include <video_player.h>
#include <zoom_view.h>
#include <Fl/Fl_Roller.H>
#include <chrono>
#include <klv_view.h>
#include <vector>

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
static Fl_Button *toggleZoomWnd=(Fl_Button *)0;
static Fl_Button *toggleKlvWnd=(Fl_Button *)0;
static Fl_Value_Slider *playTrackbar=(Fl_Value_Slider *)0;
static Fl_Input  *fileNameEdit=(Fl_Input *)0;
static Fl_Box    *uavvLibVerLabel = (Fl_Box *)0;
static Fl_Box    *uavvStreamStateLabel = (Fl_Box *)0;
static Fl_Slider *zoomSlider=(Fl_Slider *)0;
static Fl_Box    *zoomLabel = (Fl_Box *)0;

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

static Fl_Image *image_zoomin() {
  static Fl_Image *image = new Fl_RGB_Image(idata_zoomin, 32, 32, 4, 0);
  return image;
}
  
static Fl_RGB_Image *image_zoomin_mouse() {
  static Fl_RGB_Image *image = new Fl_RGB_Image(idata_zoomin_mouse, 24, 24, 4, 0);
  return image;
}

static Fl_Image *image_klvdata() {
  static Fl_Image *image = new Fl_RGB_Image(idata_klvdata, 32, 32, 4, 0);
  return image;
}

UIController::UIController() 
    : mainWnd(nullptr)
    , renderWnd(nullptr)
    , videoPlayer(nullptr)
    , imageFrameBuffer(nullptr)
    , renderWndVisible(true)
    , zoomWndVisible(false)
    , klvWndVisible(false)
    , zoomState(ZoomState::ZoomOff)
    , zoomValue(ZoomValue::x2)
{
    
}

UIController::~UIController()
{
    uavv::IUAVVInterface::DestroyImageHandle(imageFrameBuffer);
    uint8_t* tmp = static_cast<uint8_t*>(imageFrameBuffer);
    delete [] tmp;

    videoPlayer->Stop();
}

bool UIController::InitUAVVLibrary()
{
    if(!VideoPlayer::InitUAVVLibrary())
    {
        fl_message("Couldn't load or initialize UAVV library.");
        return false;
    }
    
    uavvVer = std::string("UAVV ver ") + VideoPlayer::GetUAVVVersion();
    return true;
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
    mainWnd->SetUIController(this);

    shared_ptr<RenderWnd> renderTmp(UIController::makeRenderPanel(this, FRAME_WIDTH, FRAME_HEIGHT, "Video playback"));
    renderWnd = std::move(renderTmp);

    if ((Fl::w() < renderWnd->w()) || (Fl::h() < renderWnd->h()))
        renderWnd->size(Fl::w(), Fl::h());

    int rW = renderWnd->w(); 
    int rH = renderWnd->h();
    
    int rX = (Fl::w() - renderWnd->w())/2;
    int rY = MAIN_OFFSET_Y + mainWnd->decorated_h() + RENDER_OFFSET_Y;
    renderWnd->resize(rX, rY, rW, rH);
    renderWnd->SetUIController(this);


    shared_ptr<ZoomWnd> zoomTmp(UIController::makeZoomPanel(this, FRAME_WIDTH, FRAME_HEIGHT, "Zoom Image"));
    zoomWnd = std::move(zoomTmp);

    if ((Fl::w() < zoomWnd->w()) || (Fl::h() < zoomWnd->h()))
        zoomWnd->size(Fl::w(), Fl::h());

    zoomWnd->SetUIController(this);

    shared_ptr<KLVWnd> klvTmp(UIController::makeKlvPanel(this, FRAME_WIDTH, FRAME_HEIGHT, "KLV Data"));
    klvWnd = std::move(klvTmp);

    if ((Fl::w() < klvWnd->w()) || (Fl::h() < klvWnd->h()))
        klvWnd->size(Fl::w(), Fl::h());

    klvWnd->SetUIController(this);
}

void UIController::ShowMainWindow(int argc, char *argv[])
{
    assert(mainWnd);
    assert(renderWnd);
    assert(zoomWnd);
    assert(klvWnd);

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

    if(!zoomWnd)
    {
        fl_message("ERROR: Zoom image window is not initialized");
        return;
    }

    if(!klvWnd)
    {
        fl_message("ERROR: KLV data window is not initialized");
        return;
    }

    mainWnd->show(argc, argv);
    
    if(renderWndVisible)
        renderWnd->show();

    if(zoomWndVisible)
        zoomWnd->show();

    if(klvWndVisible)
        klvWnd->show();

}

void UIController::SetupGLWindowUpdateTest()
{
    CreateGLFrameBufferTest();
    Fl::add_timeout(0, UIController::OnRenderTimeoutElapsed, static_cast<void*>(this));
}    

void UIController::SetupVideoPlayer()
{
    shared_ptr<VideoPlayer> tmpPlayer(new VideoPlayer());
    videoPlayer = std::move(tmpPlayer);

    VideoPlayerCallbackInfo info;
    info.pfnImageCallbackNotification = UIController::ImageDecodingNotification;
    info.pfnGetKlvDataNotification = UIController::KlvDataNotification;
    info.pfnPositionChangedNotification =  UIController::PositionChangeNotification;
    info.pUserData = static_cast<void*>(this);
    videoPlayer->SetCallbackInfo(info);    
}

/*static*/ UIController* UIController::CreateInstance()
{
    return new UIController();
}

/*static*/ int UIController::RunApplication(int argc, char *argv[])
{
    unique_ptr<UIController> uiController(UIController::CreateInstance());
    
    if(!uiController->InitUAVVLibrary())
        return 0;

    Fl::lock();

    uiController->InitUIComponents();
    //uiController->SetupGLWindowUpdateTest();
    uiController->SetupVideoPlayer();
    uiController->ShowMainWindow(argc,argv);
    
    // Let FLTK run
    int res = Fl::run();

    // Clean up
    Fl::unlock();

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


void UIController::FileNameChanged()
{
    Stop();
 
    std::string fileSource(fileNameEdit->value());
    if(fileSource.size() > 0)
        videoPlayer->InitPlayback(fileSource);

    zoomWnd->ClearGLFrame();
    //FirstFrame();
    
}

/*static*/ void UIController::OnChangeFileName(Fl_Widget*, void* pUserData) 
{
    assert(pUserData);
    UIController* controller = static_cast<UIController*>(pUserData); 
    if(!controller)
    {
        return;
    }

    controller->FileNameChanged();
}

/*static*/ void UIController::OnPickFile(Fl_Widget* widget, void* pUserData) 
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
        {
            if ( native.filename() ) 
            {
                fileNameEdit->value(native.filename());
            } 
            else 
            {
                fileNameEdit->value("");
            }

            OnChangeFileName(widget, pUserData);
        }
      break;
  }
}

bool UIController::IsVideoRenderVisible() const
{
    return renderWndVisible;
}

bool UIController::IsZoomWindowVisible() const
{
    return zoomWndVisible;
}

bool UIController::IsKlvWindowVisible() const
{
    return klvWndVisible;
}

void UIController::UpdateRenderWindowVisibility()
{
    assert(renderWnd);
    if(!renderWnd)
        return;

    if(renderWndVisible)
        renderWnd->show();
    else
        renderWnd->hide();
}

void UIController::UpdateZoomWindowVisibility()
{
    assert(zoomWnd);
    if(!zoomWnd)
        return;

    if(zoomWndVisible)
        zoomWnd->show();
    else
        zoomWnd->hide();
}

void UIController::UpdateKlvWindowVisibility()
{
    assert(klvWnd);
    if(!klvWnd)
        return;

    if(klvWndVisible)
        klvWnd->show();
    else
        klvWnd->hide();
}

const std::string& UIController::GetLibraryVersionString()
{
    return uavvVer;
}

const std::string& UIController::GetStreamingState()
{
    if(videoPlayer->IsPlaying() && !videoPlayer->IsPaused())
        streamingState = videoPlayer->IsVideoStreaming() ? "Video source: streaming" : "Video source: file";
    else
        streamingState = "";
        
    return streamingState;
}

void UIController::ToggleRender()
{
    renderWndVisible = !renderWndVisible;
    toggleVideoWnd->value(IsVideoRenderVisible() ? 1 : 0);
    UpdateRenderWindowVisibility();
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

void UIController::ToggleZoom()
{
    zoomWndVisible = !zoomWndVisible;
    toggleZoomWnd->value(IsZoomWindowVisible() ? 1 : 0);
    if(zoomWndVisible)
    {
        zoomState = ZoomState::ZoomIn;
        renderWnd->cursor(image_zoomin_mouse(), 10, 10);
    }    
    else
    {
        zoomState = ZoomState::ZoomOff;
        renderWnd->cursor(FL_CURSOR_DEFAULT);
    }

    UpdateZoomWindowVisibility();
}

/*static*/ void UIController::OnToggleZoom(Fl_Widget* sender, void* pUserData)
{
    assert(pUserData);
    UIController* controller = static_cast<UIController*>(pUserData); 
    if(!controller)
    {
        return;
    }

    controller->ToggleZoom();
}

void UIController::ToggleKlv()
{
    klvWndVisible = !klvWndVisible;
    toggleKlvWnd->value(IsKlvWindowVisible() ? 1 : 0);

    UpdateKlvWindowVisibility();
}

/*static*/ void UIController::OnToggleKlv(Fl_Widget* sender, void* pUserData)
{
    assert(pUserData);
    UIController* controller = static_cast<UIController*>(pUserData); 
    if(!controller)
    {
        return;
    }

    controller->ToggleKlv();
}

void UIController::FirstFrame()
{
    if(videoPlayer->GoToStart())
    {
        playBtn->show();
        pauseBtn->hide();
    }    
}

/*static*/ void UIController::OnFirstFrameClick(Fl_Widget*, void* pUserData)
{
    assert(pUserData);
    UIController* controller = static_cast<UIController*>(pUserData); 
    if(!controller)
    {
        return;
    }

    controller->FirstFrame();
}

void UIController::PreviousFrame()
{
    if(videoPlayer->StepBackward())
    {
        playBtn->show();
        pauseBtn->hide();
    }   
}

/*static*/ void UIController::OnPreviousFrameClick(Fl_Widget*, void* pUserData)
{
    assert(pUserData);
    UIController* controller = static_cast<UIController*>(pUserData); 
    if(!controller)
    {
        return;
    }

    controller->PreviousFrame();
}

void UIController::Play()
{
    videoPlayer->Play();
    if(videoPlayer->IsPlaying())
    {
        playBtn->hide();
        pauseBtn->show();
        uavvStreamStateLabel->label(GetStreamingState().c_str());
    }
    else
    {
        fl_beep();
    }
}

/*static*/ void UIController::OnPlayClick(Fl_Widget*, void* pUserData)
{
    assert(pUserData);
    UIController* controller = static_cast<UIController*>(pUserData); 
    if(!controller)
    {
        return;
    }

    controller->Play();
}

void UIController::Pause()
{
    videoPlayer->Pause();
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

    controller->Pause();
}

void UIController::Stop()
{
    videoPlayer->Stop();
    playBtn->show();
    pauseBtn->hide();

    uavvStreamStateLabel->label(GetStreamingState().c_str());
}

/*static*/ void UIController::OnStopClick(Fl_Widget*, void* pUserData)
{
    assert(pUserData);
    UIController* controller = static_cast<UIController*>(pUserData); 
    if(!controller)
    {
        return;
    }

    controller->Stop();
}

void UIController::NextFrame()
{
    if(videoPlayer->StepForward())
    {
        playBtn->show();
        pauseBtn->hide();
    }  
    else
    {
        fl_beep();
    }
}

/*static*/ void UIController::OnNextFrameClick(Fl_Widget*, void* pUserData)
{
    assert(pUserData);
    UIController* controller = static_cast<UIController*>(pUserData); 
    if(!controller)
    {
        return;
    }

    controller->NextFrame();
}

void UIController::LastFrame()
{
    if(videoPlayer->GoToEnd())
    {
        playBtn->show();
        pauseBtn->hide();
    }  
    else
    {
        fl_beep();
    }  
}

/*static*/ void UIController::OnLastFrameClick(Fl_Widget*, void* pUserData)
{
    assert(pUserData);
    UIController* controller = static_cast<UIController*>(pUserData); 
    if(!controller)
    {
        return;
    }

    controller->LastFrame();
}

void UIController::SliderPosChange(double pos)
{
    if(videoPlayer->SlideToPosition(pos))
    {
        playBtn->show();
        pauseBtn->hide();
    }
}

/*static*/ void UIController::OnSliderPosChange(Fl_Widget* widget, void* pUserData)
{
    assert(pUserData);
    UIController* controller = static_cast<UIController*>(pUserData); 
    if(!controller)
    {
        return;
    }

    Fl_Value_Slider* slider = static_cast<Fl_Value_Slider*>(widget);
    controller->SliderPosChange(slider->value());
}

const std::string& UIController::GetZoomValueString()
{
    switch(zoomValue)
    {
     case ZoomValue::x2:
        zoomValueStr = "x2";
        break;
    case ZoomValue::x4:
        zoomValueStr = "x4";
        break;
    case ZoomValue::x8:
        zoomValueStr = "x8";
        break;
    case ZoomValue::x16:
        zoomValueStr = "x16";
        break;
    case ZoomValue::x32:
        zoomValueStr = "x32";
        break;
    case ZoomValue::x64:
        zoomValueStr = "x64";
        break;   
    default:
        zoomValueStr = "x1";
    }

    return zoomValueStr;
}

void UIController::ZoomSliderPosChange(double pos)
{
    zoomValue = static_cast<ZoomValue>(static_cast<int>(pos));
    zoomLabel->label(GetZoomValueString().c_str());
    
    zoomWnd->ZoomChanged(UIController::GetZoomScaleByValue(zoomValue));
}

/*static*/ void UIController::OnZoomSliderPosChange(Fl_Widget* widget, void* pUserData)
{
    assert(pUserData);
    UIController* controller = static_cast<UIController*>(pUserData); 
    if(!controller)
    {
        return;
    }

    Fl_Slider* slider = static_cast<Fl_Slider*>(widget);
    controller->ZoomSliderPosChange(slider->value());
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
        main_panel->callback(UIController::OnCloseMainWnd, static_cast<void*>(controller));
        main_panel->align(Fl_Align(FL_ALIGN_CENTER));
        main_panel->when(FL_WHEN_RELEASE);
        { 
            fileNameEdit = new Fl_Input(25, 25, 465, 25, "Video file");
            fileNameEdit->tooltip("Video file path");
            fileNameEdit->align(Fl_Align(FL_ALIGN_TOP_LEFT));
            fileNameEdit->callback(UIController::OnChangeFileName, static_cast<void*>(controller));
        } // Fl_Input* fileNameEdit
        { 
            loadVideo = new Fl_Button(490, 25, 25, 25);
            loadVideo->tooltip("Load video file");
            loadVideo->down_box(FL_DOWN_BOX);
            loadVideo->callback(UIController::OnPickFile, static_cast<void*>(controller));
            loadVideo->image( image_open() );   
        } // Fl_Button* loadVideo
        
        { 
            firstFrameBtn = new Fl_Button(25, 125, 40, 40);
            firstFrameBtn->image( image_first_b() );
            firstFrameBtn->align(Fl_Align(512));
            firstFrameBtn->callback(UIController::OnFirstFrameClick, static_cast<void*>(controller));
        } // Fl_Button* firstFrameBtn
        { 
            prevFrameBtn = new Fl_Button(80, 125, 40, 40);
            prevFrameBtn->image( image_prev_b() );
            prevFrameBtn->align(Fl_Align(512));
            prevFrameBtn->callback(UIController::OnPreviousFrameClick, static_cast<void*>(controller));
        } // Fl_Button* prevFrameBtn
        { 
            playBtn = new Fl_Button(135, 125, 40, 40);
            playBtn->image( image_play_b() );
            playBtn->align(Fl_Align(512));
            playBtn->callback(UIController::OnPlayClick, static_cast<void*>(controller));
        } // Fl_Button* playBtn
        { 
            stopBtn = new Fl_Button(190, 125, 40, 40);
            stopBtn->image( image_stop_b() );
            stopBtn->align(Fl_Align(512));
            stopBtn->callback(UIController::OnStopClick, static_cast<void*>(controller));
        } // Fl_Button* stopBtn
        { 
            nextFrameBtn = new Fl_Button(245, 125, 40, 40);
            nextFrameBtn->image( image_next_b() );
            nextFrameBtn->align(Fl_Align(512));
            nextFrameBtn->callback(UIController::OnNextFrameClick, static_cast<void*>(controller));
        } // Fl_Button*nextFrameBtn
        { 
            lastFrameBtn = new Fl_Button(300, 125, 40, 40);
            lastFrameBtn->image( image_last_b() );
            lastFrameBtn->align(Fl_Align(512));
            lastFrameBtn->callback(UIController::OnLastFrameClick, static_cast<void*>(controller));
        } // Fl_Button* lastFrameBtn
        { 
            pauseBtn = new Fl_Button(135, 125, 40, 40);
            pauseBtn->image( image_pause_b() );
            pauseBtn->hide();
            pauseBtn->callback(UIController::OnPauseClick, static_cast<void*>(controller));
        } // Fl_Button* pauseBtn

        { 
            playTrackbar = new Fl_Value_Slider(20, 75, 495, 27);
            playTrackbar->type(5);
            playTrackbar->box(FL_FLAT_BOX);
            playTrackbar->minimum(0.0);
            playTrackbar->maximum(100.0);
            playTrackbar->step(0.01);
            playTrackbar->callback(UIController::OnSliderPosChange, static_cast<void*>(controller));
        } // Fl_Slider* playTrackbar
        
        { 
            toggleKlvWnd = new Fl_Button(375, 125, 40, 40);
            toggleKlvWnd->type(1);
            toggleKlvWnd->down_box(FL_DOWN_BOX);
            toggleKlvWnd->selection_color((Fl_Color)55);
            toggleKlvWnd->image( image_klvdata() );
            toggleKlvWnd->align(Fl_Align(512));
            toggleKlvWnd->value(controller->IsKlvWindowVisible() ? 1 : 0);
            toggleKlvWnd->callback(UIController::OnToggleKlv, static_cast<void*>(controller));
            toggleKlvWnd->tooltip("Show/Hide KLV data window");
        } // Fl_Button* toggleVideoWnd

        { 
            toggleZoomWnd = new Fl_Button(425, 125, 40, 40);
            toggleZoomWnd->type(1);
            toggleZoomWnd->down_box(FL_DOWN_BOX);
            toggleZoomWnd->selection_color((Fl_Color)55);
            toggleZoomWnd->image( image_zoomin() );
            toggleZoomWnd->align(Fl_Align(512));
            toggleZoomWnd->value(controller->IsZoomWindowVisible() ? 1 : 0);
            toggleZoomWnd->callback(UIController::OnToggleZoom, static_cast<void*>(controller));
            toggleZoomWnd->tooltip("Show/Hide zoom window");
        } // Fl_Button* toggleZoomWnd

        { 
            toggleVideoWnd = new Fl_Button(475, 125, 40, 40);
            toggleVideoWnd->type(1);
            toggleVideoWnd->down_box(FL_DOWN_BOX);
            toggleVideoWnd->selection_color((Fl_Color)55);
            toggleVideoWnd->image( image_toggle() );
            toggleVideoWnd->align(Fl_Align(512));
            toggleVideoWnd->value(controller->IsVideoRenderVisible() ? 1 : 0);
            toggleVideoWnd->callback(UIController::OnToggleRender, static_cast<void*>(controller));
            toggleVideoWnd->tooltip("Show/Hide playback window");
        } // Fl_Button* toggleVideoWnd

        { 
            uavvLibVerLabel = new Fl_Box( FL_FLAT_BOX, 5, 170, 100, 20, controller->GetLibraryVersionString().c_str());
            uavvLibVerLabel->labeltype(FL_NORMAL_LABEL);
            uavvLibVerLabel->align(FL_ALIGN_INSIDE | FL_ALIGN_LEFT);
            uavvLibVerLabel->labelsize(10);
        } // Fl_Box* uavvLibVerLabel

        { 
            uavvStreamStateLabel = new Fl_Box( FL_FLAT_BOX, 110, 170, 100, 20, nullptr);
            uavvStreamStateLabel->labeltype(FL_NORMAL_LABEL);
            uavvStreamStateLabel->align(FL_ALIGN_INSIDE | FL_ALIGN_LEFT);
            uavvStreamStateLabel->labelsize(10);
        } // Fl_Box* uavvStreamStateLabel

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
        o->callback(UIController::OnToggleRender, static_cast<void*>(controller));
        o->set_non_modal();
        o->end();
        o->resizable(o);
    } // RenderWnd* o
    return w;
}

/*static*/ ZoomWnd* UIController::makeZoomPanel(UIController* controller, int W, int H, const char* l)
{
    ZoomWnd* w = nullptr;;
    ZoomView* zoomView = nullptr;
    Fl_Group* MainView = nullptr;
    {
        ZoomWnd* z = new ZoomWnd(W, H, l);
        w = z;
        z->box(FL_FLAT_BOX);
        z->labeltype(FL_NO_LABEL);
        z->labelsize(14);
        z->when(FL_WHEN_RELEASE);
        z->callback(UIController::OnToggleZoom, static_cast<void*>(controller));
        z->set_non_modal();

        { 
            { 
                zoomSlider = new Fl_Slider(50, 5, 300, 27, "Zoom");
                zoomSlider->type(5);
                zoomSlider->box(FL_FLAT_BOX);
                zoomSlider->minimum(static_cast<int>(ZoomValue::x1));
                zoomSlider->maximum(static_cast<int>(ZoomValue::ZoomValueLast) - 1);
                zoomSlider->step(1);
                zoomSlider->value(static_cast<int>(controller->GetCurrentZoomValue()));
                zoomSlider->callback(UIController::OnZoomSliderPosChange, static_cast<void*>(controller));
                zoomSlider->align(Fl_Align(FL_ALIGN_LEFT));
            } // Fl_Slider* zoom
            { 
                zoomLabel = new Fl_Box( FL_FLAT_BOX, 360, 8, 50, 20, controller->GetZoomValueString().c_str());
                zoomLabel->labeltype(FL_NORMAL_LABEL);
                zoomLabel->align(FL_ALIGN_INSIDE | FL_ALIGN_LEFT);
            } // Fl_Box* zoomLabel
            { 
                MainView = new Fl_Group(10, 35, W-20, H-45);
                { 
                    zoomView = new ZoomView(10, 35, W-20, H-45, "This is the Zoom image window");
                    zoomView->box(FL_NO_BOX);
                    zoomView->color(FL_BACKGROUND_COLOR);
                    zoomView->selection_color(FL_BACKGROUND_COLOR);
                    zoomView->labeltype(FL_NORMAL_LABEL);
                    zoomView->labelfont(0);
                    zoomView->labelsize(14);
                    zoomView->labelcolor(FL_FOREGROUND_COLOR);
                    zoomView->align(Fl_Align(FL_ALIGN_CENTER|FL_ALIGN_INSIDE));
                    zoomView->when(FL_WHEN_RELEASE);
                    zoomView->SetUIController(controller);
                    Fl_Group::current()->resizable(zoomView);
                } // ZoomView* zoomView
                MainView->end();
            } // Fl_Group* MainView
        } // Fl_Group* o
        z->end();
        z->size_range(W, H, 0, 0);
        z->resizable(MainView);
        z->SetView(zoomView);
    } // Fl_Double_Window* mainWindow

    assert(w);
    assert(zoomView);
    assert(MainView);
    return w;
}

/*static*/ KLVWnd* UIController::makeKlvPanel(UIController* controller, int W, int H, const char* l)
{
    KLVWnd* w = nullptr;
    KLVTableView* klvtable = nullptr;
    { 
        KLVWnd* o = new KLVWnd(W, H, l);
        w = o; if (w) {/* empty */}
        o->box(FL_FLAT_BOX);
        o->color(FL_BACKGROUND_COLOR);
        o->selection_color(FL_BACKGROUND_COLOR);
        o->labeltype(FL_NO_LABEL);
        o->labelfont(0);
        o->labelsize(14);
        o->labelcolor(FL_FOREGROUND_COLOR);
        o->when(FL_WHEN_RELEASE);
        o->callback(UIController::OnToggleKlv, static_cast<void*>(controller));
        o->set_non_modal();
        {
            klvtable = new KLVTableView(20, 20, W-40, H-40, "KLV Data");
            klvtable->selection_color(FL_LIGHT1);
            klvtable->when(FL_WHEN_RELEASE | FL_WHEN_CHANGED);
            klvtable->table_box(FL_NO_BOX);
            klvtable->col_resize_min(4);
            klvtable->row_resize_min(4);

            // ROWS
            klvtable->row_header(0);
            klvtable->row_header_width(60);
            klvtable->row_resize(1);
            klvtable->rows(MAX_UAVV_KLV_KEY);
            klvtable->row_height_all(20);

            // COLS
            int colWidth = klvtable->w()-10;
            klvtable->cols(3);
            klvtable->col_header(1);
            klvtable->col_header_height(25);
            klvtable->col_resize(1);
            klvtable->col_width(0, colWidth * 0.4);
            klvtable->col_width(1, colWidth * 0.5);
            klvtable->col_width(2, colWidth * 0.1);
        }

        o->end();
        o->size_range(W, H, 0, 0);
        o->resizable(klvtable);
        o->SetKLVDataView(klvtable);
    } // KLVWnd* o
    return w;
}

void UIController::ExitApplication()
{
    if(klvWnd->shown())
        klvWnd->hide();

    if(zoomWnd->shown())
        zoomWnd->hide();

    if(renderWnd->shown())
        renderWnd->hide();

    klvWnd->CleanUp();
    zoomWnd->CleanUp();
    renderWnd->CleanUp();

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

    controller->ExitApplication();
}

void UIController::draw_bresenham_line(UAVV_IMAGE buf, int x1, int y1, int x2, int y2)
{
    uint8_t* data = (uint8_t*) uavv::IUAVVInterface::GetImageData(buf);
    int dx = abs(x2 - x1);
    int dy = -abs(y2 - y1);
    int sx = (x1 < x2) ? 1 : -1;
    int sy = (y1 < y2) ? 1 : -1;
    int err = dx + dy;
    int e2;
    for (;;)
    {
        // This sets the pixel red.
    
        *(data + 4*x1 + 4*uavv::IUAVVInterface::GetImageWidth(buf)*y1) = 255;      // R
        *(data + 4*x1 + 4*uavv::IUAVVInterface::GetImageWidth(buf)*y1 + 1) = 0;    // G
        *(data + 4*x1 + 4*uavv::IUAVVInterface::GetImageWidth(buf)*y1 + 2) = 0;    // B
        *(data + 4*x1 + 4*uavv::IUAVVInterface::GetImageWidth(buf)*y1 + 3) = 255;  // A

        if ((x1 == x2) && (y1 == y2))  break;
        e2 = 2 * err;
        if (e2 <= dx)
        {
            err += dx;
            y1  += sy;
        }
        if (e2 >= dy)
        {
            err += dy;
            x1  += sx;
        }
    }
}

UAVV_IMAGE UIController::CreateGLFrameBufferTest()
{
    // Create an empty canvas
    imageFrameBuffer = uavv::IUAVVInterface::CreateImageHandle(FRAME_WIDTH, FRAME_HEIGHT);
    memset(uavv::IUAVVInterface::GetImageData(imageFrameBuffer), 200, uavv::IUAVVInterface::GetImageLength(imageFrameBuffer));

    draw_bresenham_line(imageFrameBuffer, 0, 0, FRAME_WIDTH-1, FRAME_HEIGHT-1);
    draw_bresenham_line(imageFrameBuffer, FRAME_WIDTH-1, 0, 0, FRAME_HEIGHT-1);

    return imageFrameBuffer;
}

void UIController::UpdateGLFrameBufferTest()
{
    const UAVV_IMAGE buf = static_cast<const UAVV_IMAGE>(imageFrameBuffer);
    renderWnd->UpdateGLFrame(buf);
    Fl::repeat_timeout(0, UIController::OnRenderTimeoutElapsed, static_cast<void*>(this));
}

/*static*/ void UIController::OnRenderTimeoutElapsed(void* pUserData)
{
    assert(pUserData);
    UIController* controller = static_cast<UIController*>(pUserData); 
    if(!controller)
    {
        return;
    }

    controller->UpdateGLFrameBufferTest();
}

void UIController::ImageBufferReceived(UAVV_IMAGE img, int delay, float pos)
{
    renderWnd->UpdateGLFrame(img);

    Fl::awake();
}

/*static*/ void UIController::ImageDecodingNotification(UAVV_IMAGE img, int delay, float pos, void* pUserData)
{
    UIController* controller = static_cast<UIController*>(pUserData); 
    if(!controller)
    {
        return;
    }

    controller->ImageBufferReceived(img, delay, pos);
}

void UIController::KlvDataReceived(UAVV_KLV klvDataHandle)
{
    //FILE* file = fopen("klv.log", "wb");
    //if (file)
    klvItems.clear();
    int count = IUAVVInterface::KlvSize(klvDataHandle);
    if(count > 0)
    {
        int count = IUAVVInterface::KlvSize(klvDataHandle);
        klvItems.resize(count + 1);

        // Obtain KLV timestamp
        long long timestamp = IUAVVInterface::KlvGetTimeStamp(klvDataHandle);
        time_t    sec = timestamp / 1000000;
        long      usec = timestamp % 1000000;

        // Format UTC timestamp
        struct tm tm;
        gmtime_r(&sec, &tm);
        char tstr[30];
        int len = strftime(tstr, sizeof(tstr), "%Y/%m/%d %H:%M:%S", &tm);
        sprintf(tstr + len, ".%03ld ", usec / 1000);

        klvItems[0].itemName = "Time";
        klvItems[0].itemValue = std::string(tstr) + " UTC";
        klvItems[0].itemState = "OK";
        
        //fprintf(file, "-- %s UTC --\n", tstr);

        // Dump all KLV items to file
        for (int i = 1;  i <= count;  ++i)
        {
            uavv_klv_key_t key = IUAVVInterface::KlvItem(klvDataHandle, i);
            klvItems[i].itemName = IUAVVInterface::KlvItemName(key);
            klvItems[i].itemValue = IUAVVInterface::KlvGetString(klvDataHandle, key);
            klvItems[i].itemState = IUAVVInterface::KlvIsInErrorState(klvDataHandle, key) ? "Error" : "Ok";
        }
        //fclose(file);
    }

    klvWnd->UpdateKLVData(klvItems);

    Fl::awake();
} 

/*static*/ void UIController::KlvDataNotification(UAVV_KLV klvData, void* pUserData)
{
    UIController* controller = static_cast<UIController*>(pUserData); 
    if(!controller)
    {
        return;
    }

    controller->KlvDataReceived(klvData);
}

void UIController::UpdatePosition(float pos)
{
    playTrackbar->value(static_cast<double>(pos));
}

/*static*/ void UIController::PositionChangeNotification(float pos, void* pUserData)
{
    UIController* controller = static_cast<UIController*>(pUserData); 
    if(!controller)
    {
        return;
    }

    controller->UpdatePosition(pos);
}

void UIController::UpdatePlayerControls()
{
    UpdatePosition(videoPlayer->GetCurrentPosition());
}

void UIController::OnRenderMouseDown(int event, float scaledX, float scaledY)
{
    if(zoomState == ZoomState::ZoomIn)
    {
        const UAVV_IMAGE buf = static_cast<const UAVV_IMAGE>(renderWnd->GetCurrentFrameCopy());
        zoomWnd->UpdateGLFrame(buf, scaledX, scaledY, UIController::GetZoomScaleByValue(zoomValue));
        IUAVVInterface::DestroyImageHandle(buf);
    }
}

ZoomValue UIController::GetCurrentZoomValue() const
{
    return zoomValue;
}

void UIController::UpdateCurrentZoomValue(ZoomValue newValue)
{
    ZoomValue val = newValue;
    assert(val >= ZoomValue::x1 && val < ZoomValue::ZoomValueLast);

    if(val < ZoomValue::x1)
    {
        val = ZoomValue::x1;
    }
    
    if(val >= ZoomValue::ZoomValueLast)
    {
        val = static_cast<ZoomValue>(static_cast<int>(ZoomValue::ZoomValueLast) - 1);
    }

    zoomValue = val;
    zoomLabel->label(GetZoomValueString().c_str());
    zoomSlider->value(static_cast<double>(static_cast<int>(zoomValue)));
}

/*static*/ int UIController::GetZoomScaleByValue(ZoomValue val)
{
    int zoomParam = 1 << static_cast<int>(val);
    return zoomParam;
}