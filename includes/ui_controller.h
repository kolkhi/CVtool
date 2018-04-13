#ifndef ui_controller_h
#define ui_controller_h

#include <FL/Fl.H>
#include <memory>
#include <main_panel.h>
#include <render_panel.h>
#include <zoom_panel.h>
#include <uavv_image.h>
#include <data_structures.h>

#define FRAME_WIDTH             800
#define FRAME_HEIGHT            600

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
        shared_ptr<VideoPlayer> videoPlayer;

        UAVV_IMAGE imageFrameBuffer;
        bool renderWndVisible;
        bool zoomWndVisible;

        std::string uavvVer;
        std::string streamingState;
        
        ZoomState zoomState;
        ZoomValue zoomValue;
        std::string zoomValueStr;

        protected:
            static MainWnd* makeMainPanel(UIController* controller);
            static RenderWnd* makeRenderPanel(UIController* controller, int W, int H, const char* l = 0);
            static ZoomWnd* makeZoomPanel(UIController* controller, int W, int H, const char* l = 0);

            void UpdateRenderWindowVisibility();
            void UpdateZoomWindowVisibility();

            UIController();

            UAVV_IMAGE CreateGLFrameBufferTest();
            void draw_bresenham_line(UAVV_IMAGE buf, int x1, int y1, int x2, int y2);

            const std::string& GetLibraryVersionString();
            const std::string& GetStreamingState();
            const std::string& GetZoomValueString();

        public:

            ~UIController();

            // contoller management functions
            void InitUIComponents();
            bool InitUAVVLibrary();
            void ShowMainWindow(int argc, char *argv[]);
            bool IsVideoRenderVisible() const;
            bool IsZoomWindowVisible() const;
            ZoomValue GetCurrentZoomValue() const;
            void SetupGLWindowUpdateTest();
            void SetupVideoPlayer();
            
            // button click handlers
            void ToggleRender();
            void ToggleZoom();
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

            void ExitApplication();
            void UpdateGLFrameBufferTest();
            void UpdateCurrentZoomValue(ZoomValue newValue);
            void ImageBufferReceived(UAVV_IMAGE img, int delay, float pos);
            void UpdatePosition(float pos);
            void UpdatePlayerControls();
            
            void OnRenderMouseDown(int event, float scaledX, float scaledY);

            static UIController* CreateInstance();

            static int RunApplication(int argc, char *argv[]);
            static void SetupSystemSettings();
            static int GetZoomScaleByValue(ZoomValue val);

            // widget event static handlers
            static void OnPickFile(Fl_Widget*, void*);
            static void OnToggleRender(Fl_Widget*, void*);
            static void OnToggleZoom(Fl_Widget* sender, void*);
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
            static void OnZoomSliderPosChange(Fl_Widget* widget, void*);

            // video player callbacks 
            static void ImageDecodingNotification(UAVV_IMAGE img, int delay, float pos, void* pUserData);
            static void PositionChangeNotification(float pos, void* pUserData);
    };
}

#endif

