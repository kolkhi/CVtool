#ifndef draw_controller_h
#define draw_controller_h

#include <draw_model.h>
#include <Fl/Fl_Menu_Item.H>

using namespace std;

namespace cvtool
{
    class UIController;
    class DrawController
    {
        DrawModel model;
        UIController* uiController;
        
        protected:
            DrawController(); 

            std::string GetLineWidthString(LineWidth lineWidth);

            void ChangeLineWidth(int index);
            bool ChooseNewColor();
            void ClearDrawing();

            void Redraw();
        public:

            ~DrawController();

            const Fl_Menu_Item* GetLineWidthChoiceItems();
            const DrawModel& GetModel();

            // contoller management functions
            static DrawController* CreateInstance();
            void SetUIController(UIController* controller);

            void SetCurrentColor(Fl_Color color);
            Fl_Color GetCurrentColor() const;

            void SetCurrentLineWidth(LineWidth width);
            LineWidth GetCurrentLineWidth() const;

            // widget event static handlers
            static void OnDrawLine(Fl_Widget* sender, void* pUserData);
            static void OnDrawRect(Fl_Widget* sender, void* pUserData);
            static void OnDrawClear(Fl_Widget* sender, void* pUserData);
            static void OnChooseColor(Fl_Widget* sender, void* pUserData);
            static void OnChangeLineWidth(Fl_Widget* sender, void* pUserData);
    };
}

#endif

