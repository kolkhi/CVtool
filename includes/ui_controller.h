#ifndef ui_controller_h
#define ui_controller_h

#include <FL/Fl.H>
#include <memory>
#include <main_panel.h>
#include <render_panel.h>
#include <uavv_image.h>

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
        shared_ptr<VideoPlayer> videoPlayer;

        UAVV_IMAGE imageFrameBuffer;
        bool renderWndVisible;
        std::string uavvVer;
        
        protected:
            static MainWnd* makeMainPanel(UIController* controller);
            static RenderWnd* makeRenderPanel(UIController* controller, int W, int H, const char* l = 0);

            void UpdateRenderVisibility();

            UIController();

            UAVV_IMAGE CreateGLFrameBufferTest();
            void draw_bresenham_line(UAVV_IMAGE buf, int x1, int y1, int x2, int y2);
        public:
        
            ~UIController();

            // contoller management functions
            void InitUIComponents();
            bool InitUAVVLibrary();
            void ShowMainWindow(int argc, char *argv[]);
            bool IsVideoRenderVisible() const;
            void SetupGLWindowUpdateTest();
            void SetupVideoPlayer();
            const std::string& GetLibraryVersionString();
            
            // button click handlers
            void ToggleRender();
            void FirstFrameClick();
            void PreviousFrameClick();
            void PlayClick();
            void PauseClick();
            void StopClick();
            void NextFrameClick();
            void LastFrameClick();
            void SliderPosChange(double pos);

            void ExitApplication();
            void UpdateGLFrameBufferTest();
            void ImageBufferReceived(UAVV_IMAGE img, int delay, float pos);
            
            static UIController* CreateInstance();

            static int RunApplication(int argc, char *argv[]);
            static void SetupSystemSettings();

            // widget event static handlers
            static void OnPickFile(Fl_Widget*, void*);
            static void OnToggleRender(Fl_Widget*, void*);
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

            // video player callbacks 
            static void ImageDecoding(UAVV_IMAGE img, int delay, float pos, void* pUserData);
    };
}

#endif

