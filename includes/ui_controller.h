#ifndef ui_controller_h
#define ui_controller_h

#include <FL/Fl.H>
#include <memory>
#include <main_panel.h>
#include <render_panel.h>

using namespace std;

namespace cvtool
{
    class UIController
    {
        const int MAIN_OFFSET_Y = 100; 
        const int RENDER_OFFSET_Y = 50;

        shared_ptr<MainWnd> mainWnd;
        shared_ptr<RenderWnd> renderWnd;
        bool renderWndVisible;
        
        protected:
            static MainWnd* makeMainPanel(UIController* controller);
            static RenderWnd* makeRenderPanel(UIController* controller, int W, int H, const char* l = 0);

            void UpdateRenderVisibility();

            UIController();

        public:
        
            ~UIController();

            // contoller management functions
            void InitUIComponents();
            void ShowMainWindow(int argc, char *argv[]);
            bool IsVideoRenderVisible() const;

            // button click handlers
            void ToggleRender();
            void FirstFrameClick();
            void PreviousFrameClick();
            void PlayClick();
            void PauseClick();
            void StopClick();
            void NextFrameClick();
            void LastFrameClick();
            void SliderPosChange(int pos);

            void ExitApplicaion();
            
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
    };
}

#endif

