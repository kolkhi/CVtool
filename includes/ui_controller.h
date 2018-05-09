#ifndef ui_controller_h
#define ui_controller_h

#include <FL/Fl.H>
#include <memory>
#include <main_panel.h>
#include <render_panel.h>
#include <zoom_panel.h>
#include <klv_panel.h>
#include <plot_panel.h>
#include <uavv_image.h>
#include <data_structures.h>
#include <vector>
#include <deque>
#include <draw_controller.h>
#include <thumbnails_controller.h>

#define FRAME_WIDTH             800
#define FRAME_HEIGHT            600

#define THUMBNAIL_WIDTH         60
#define THUMBNAIL_HEIGHT        60
#define THUMBNAIL_OFFSET        10

using namespace std;
namespace cvtool
{
    class VideoPlayer;
    class UIController
    {
        const int MAIN_OFFSET_Y = 100; 
        const int RENDER_OFFSET_Y = 50;

        shared_ptr<MainWnd> mainWnd;
        shared_ptr<RenderWnd> renderWnd;
        shared_ptr<ZoomWnd> zoomWnd;
        shared_ptr<KLVWnd> klvWnd;
        shared_ptr<PlotWnd> plotWnd;
        shared_ptr<VideoPlayer> videoPlayer;

        UAVV_IMAGE imageFrameBuffer;
        bool renderWndVisible;
        bool zoomWndVisible;
        bool klvWndVisible;
        bool plotWndVisible;

        std::string uavvVer;
        std::string streamingState;
        
        ZoomMode zoomState;
        ZoomValue zoomValue;
        std::string zoomValueStr;
        std::vector<KLVItem> klvItems;

        shared_ptr<DrawController> drawController;
        shared_ptr<ThumbnailsController> thumbnailsController;
        
        protected:
            static MainWnd* makeMainPanel(UIController* controller);
            static RenderWnd* makeRenderPanel(UIController* controller, int W, int H, const char* l = 0);
            static ZoomWnd* makeZoomPanel(UIController* controller, int W, int H, const char* l = 0);
            static KLVWnd* makeKlvPanel(UIController* controller, int W, int H, const char* l = 0);
            static PlotWnd* makePlotPanel(UIController* controller, int W, int H, const char* l = 0);

            void UpdateRenderWindowVisibility();
            void UpdateZoomWindowVisibility();
            void UpdateKlvWindowVisibility();
            void UpdatePlotWindowVisibility();

            UIController();

            UAVV_IMAGE CreateGLFrameBufferTest();
            void draw_bresenham_line(UAVV_IMAGE buf, int x1, int y1, int x2, int y2);

            const std::string& GetLibraryVersionString();
            const std::string& GetStreamingState();
            const std::string& GetZoomValueString();
            
            void ImageBufferReceived(UAVV_IMAGE img, int delay, float pos);
            void KlvDataReceived(UAVV_KLV klvData);
            
            void SaveLayout();
            void LoadLayout();
            void LoadRenderWindowLayout(const Json::Value& layout);
            void LoadZoomWindowLayout(const Json::Value& layout);
            void LoadKLVWindowLayout(const Json::Value& layout);
            void LoadPlotWindowLayout(const Json::Value& layout);

            void UpdateImage(UAVV_IMAGE img);
            void SetZoomMode();
        public:

            ~UIController();

            // contoller management functions
            void InitUIComponents();
            bool InitUAVVLibrary();
            void ShowMainWindow(int argc, char *argv[]);
            bool IsVideoRenderVisible() const;
            bool IsZoomWindowVisible() const;
            bool IsKlvWindowVisible() const;
            bool IsPlotWindowVisible() const;
            ZoomValue GetCurrentZoomValue() const;
            void SetupGLWindowUpdateTest();
            void SetupVideoPlayer();
            shared_ptr<DrawController> GetDrawController();
            shared_ptr<ThumbnailsController> GetThumbnailsController();
            void UpdateDrawing();
            void RedrawThumbnails();
            void ShowImage(UAVV_IMAGE img);
            
            // button click handlers
            void ToggleRender();
            void ToggleZoom();
            void ToggleKlv();
            void TogglePlot();
            
            void FirstFrame();
            void PreviousFrame();
            void Play();
            void Pause();
            void Stop();
            void NextFrame();
            void LastFrame();
            void SliderPosChange(double pos);
            void ZoomSliderPosChange(double pos);
            void FileNameChanged();
            void PasteImage(Fl_RGB_Image* img);

            void ExitApplication();
            void UpdateGLFrameBufferTest();
            void UpdateCurrentZoomValue(ZoomValue newValue);
            void UpdatePosition(float pos);
            void UpdatePlayerControls();
            void UpdateDrawingButtons();
            void PasteClipboard();
            UAVV_IMAGE ConvertImage(Fl_RGB_Image* img);
            Fl_RGB_Image* ConvertImage(UAVV_IMAGE img);

            // windows event handlers
            void OnRenderMouseDown(float scaledX, float scaledY);
            void OnZoomMouseLeftDown(float scaledX, float scaledY);
            void OnZoomMouseLeftUp(float scaledX, float scaledY);
            void OnZoomMouseRightDown(float scaledX, float scaledY);
            void OnZoomMouseRightUp(float scaledX, float scaledY);
            void OnZoomMouseMove(float scaledX, float scaledY);

            static UIController* CreateInstance();

            static int RunApplication(int argc, char *argv[]);
            static void SetupSystemSettings();
            static int GetZoomScaleByValue(ZoomValue val);

            // widget event static handlers
            static void OnPickFile(Fl_Widget*, void*);
            static void OnPasteClipboard(Fl_Widget*, void*);
            static void OnToggleRender(Fl_Widget*, void*);
            static void OnToggleZoom(Fl_Widget*, void*);
            static void OnToggleKlv(Fl_Widget*, void*);
            static void OnTogglePlot(Fl_Widget*, void*);
            static void OnFirstFrameClick(Fl_Widget*, void*);
            static void OnPreviousFrameClick(Fl_Widget*, void*);
            static void OnPlayClick(Fl_Widget*, void*);
            static void OnPauseClick(Fl_Widget*, void*);
            static void OnStopClick(Fl_Widget*, void*);
            static void OnNextFrameClick(Fl_Widget*, void*);
            static void OnLastFrameClick(Fl_Widget*, void*);
            static void OnCloseMainWnd(Fl_Widget*, void*);
            static void OnSliderPosChange(Fl_Widget*, void*);
            static void OnRenderTimeoutElapsed(void*);
            static void OnChangeFileName(Fl_Widget*, void*); 
            static void OnZoomSliderPosChange(Fl_Widget*, void*);          

            // video player callbacks 
            static void ImageDecodingNotification(UAVV_IMAGE img, int delay, float pos, void* pUserData);
            static void KlvDataNotification(UAVV_KLV klvData, void* pUserData);
            static void PositionChangeNotification(float pos, void* pUserData);
    };
}

#endif

