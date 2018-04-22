#ifndef ui_controller_h
#define ui_controller_h

#include <FL/Fl.H>
#include <memory>
#include <main_panel.h>
#include <render_panel.h>
#include <zoom_panel.h>
#include <klv_panel.h>
#include <uavv_image.h>
#include <data_structures.h>
#include <vector>
#include <draw_controller.h>

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
        shared_ptr<KLVWnd> klvWnd;
        shared_ptr<VideoPlayer> videoPlayer;

        UAVV_IMAGE imageFrameBuffer;
        bool renderWndVisible;
        bool zoomWndVisible;
        bool klvWndVisible;

        std::string uavvVer;
        std::string streamingState;
        
        ZoomState zoomState;
        ZoomValue zoomValue;
        std::string zoomValueStr;
        std::vector<KLVItem> klvItems;

        shared_ptr<DrawController> drawController;

        protected:
            static MainWnd* makeMainPanel(UIController* controller);
            static RenderWnd* makeRenderPanel(UIController* controller, int W, int H, const char* l = 0);
            static ZoomWnd* makeZoomPanel(UIController* controller, int W, int H, const char* l = 0);
            static KLVWnd* makeKlvPanel(UIController* controller, int W, int H, const char* l = 0);

            void UpdateRenderWindowVisibility();
            void UpdateZoomWindowVisibility();
            void UpdateKlvWindowVisibility();

            UIController();

            UAVV_IMAGE CreateGLFrameBufferTest();
            void draw_bresenham_line(UAVV_IMAGE buf, int x1, int y1, int x2, int y2);

            const std::string& GetLibraryVersionString();
            const std::string& GetStreamingState();
            const std::string& GetZoomValueString();
            
            void ImageBufferReceived(UAVV_IMAGE img, int delay, float pos);
            void KlvDataReceived(UAVV_KLV klvData);
            
        public:

            ~UIController();

            // contoller management functions
            void InitUIComponents();
            bool InitUAVVLibrary();
            void ShowMainWindow(int argc, char *argv[]);
            bool IsVideoRenderVisible() const;
            bool IsZoomWindowVisible() const;
            bool IsKlvWindowVisible() const;
            ZoomValue GetCurrentZoomValue() const;
            void SetupGLWindowUpdateTest();
            void SetupVideoPlayer();
            shared_ptr<DrawController> GetDrawController();
            void UpdateDrawing();
            
            // button click handlers
            void ToggleRender();
            void ToggleZoom();
            void ToggleKlv();
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
            void UpdatePosition(float pos);
            void UpdatePlayerControls();
            void UpdateDrawingButtons();
            
            // windows event handlers
            void OnRenderMouseDown(float scaledX, float scaledY);
            void OnZoomMouseDown(float scaledX, float scaledY);
            void OnZoomMouseUp(float scaledX, float scaledY);
            void OnZoomMouseMove(float scaledX, float scaledY);

            static UIController* CreateInstance();

            static int RunApplication(int argc, char *argv[]);
            static void SetupSystemSettings();
            static int GetZoomScaleByValue(ZoomValue val);

            // widget event static handlers
            static void OnPickFile(Fl_Widget*, void*);
            static void OnToggleRender(Fl_Widget*, void*);
            static void OnToggleZoom(Fl_Widget*, void*);
            static void OnToggleKlv(Fl_Widget*, void*);
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
            static void KlvDataNotification(UAVV_KLV klvData, void* pUserData);
            static void PositionChangeNotification(float pos, void* pUserData);
    };
}

#endif

