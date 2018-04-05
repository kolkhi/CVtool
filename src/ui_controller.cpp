#include <ui_controller.h>
#include <resources.h>
#include <cassert>
#include <FL/Fl_Native_File_Chooser.H>
#include <FL/Fl_Input.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_Slider.H>
#include <FL/fl_ask.H>
#include <Fl/Fl_Box.H>
#include <video_player.h>
#include <chrono>

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
static Fl_Box    *uavvLibVerLabel = (Fl_Box *)0;
static Fl_Box    *uavvStreamStateLabel = (Fl_Box *)0;

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
    , videoPlayer(nullptr)
    , imageFrameBuffer(nullptr)
    , renderWndVisible(1)
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
    info.pfnImageCallback = UIController::ImageDecodingNotification;
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

    FirstFrame();
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
    videoPlayer->SlideToPosition(pos);
}

/*static*/ void UIController::OnSliderPosChange(Fl_Widget* widget, void* pUserData)
{
    assert(pUserData);
    UIController* controller = static_cast<UIController*>(pUserData); 
    if(!controller)
    {
        return;
    }

    Fl_Slider* slider = static_cast<Fl_Slider*>(widget);
    controller->SliderPosChange(slider->value());
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
            playTrackbar = new Fl_Slider(25, 75, 490, 27);
            playTrackbar->type(5);
            playTrackbar->box(FL_FLAT_BOX);
            playTrackbar->maximum(100.0);
            playTrackbar->step(0.01);
            playTrackbar->callback(UIController::OnSliderPosChange, static_cast<void*>(controller));
        } // Fl_Slider* playTrackbar
        
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
        } // Fl_Button* toggleVideoWnd

        { 
            uavvStreamStateLabel = new Fl_Box( FL_FLAT_BOX, 110, 170, 100, 20, nullptr);
            uavvStreamStateLabel->labeltype(FL_NORMAL_LABEL);
            uavvStreamStateLabel->align(FL_ALIGN_INSIDE | FL_ALIGN_LEFT);
            uavvStreamStateLabel->labelsize(10);
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
        o->callback(UIController::OnToggleRender, static_cast<void*>(controller));
        o->set_non_modal();
        o->end();
        o->resizable(o);
    } // RenderWnd* o
    return w;
}

void UIController::ExitApplication()
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
    
    if(videoPlayer->IsPlaying())
        videoPlayer->SetCurrentPosition(pos);

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