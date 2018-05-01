#ifndef draw_controller_h
#define draw_controller_h

#include <draw_model.h>
#include <Fl/Fl_Menu_Item.H>
#include <json/json.h>
#include <deque>

#define UNDO_LIMIT 10

using namespace std;

namespace cvtool
{
    class UIController;
    class DrawController
    {
        DrawModel model;
        UIController* uiController;
        DrawingMode drawingMode;
        int mouseClickCount;
        Figure2f* previewFigure;

        std::deque<DrawModel> undoCollection;

        protected:
            DrawController(); 

            std::string GetLineWidthString(LineWidth lineWidth);

            void ChangeLineWidth(int index);
            bool ChooseNewColor();
            void RemoveLastAddedFigure();

            void Redraw();

            void EnableLineDrawingMode();
            void EnableRectDrawingMode();
            void EnablePolygonDrawingMode();
            void DisableDrawing();
            void ResetDrawingParameters();

            void drawLines();
            void drawRectangles();
            void drawPolygons();
            void drawPreview();

            void drawLine(const Line2f* line);
            void drawRect(const Rect2f* rect);
            void drawPolygon(const Polygon2f* polygon);

            void AddToUndo();
            bool Undo();

            void LoadGeometryFromFile(const std::string& filePath);
            void SaveGeometryToFile(const std::string& filePath);    

            void ReadLatestVersion(const Json::Value& val);
            bool ParseLine(const Json::Value& value, Line2f& line);
            bool ParseRect(const Json::Value& value, Rect2f& rect);
            bool ParsePoly(const Json::Value& value, Polygon2f& poly);

            void SaveLines(Json::Value& root);
            void SaveRects(Json::Value& root);
            void SavePolys(Json::Value& root);
        public:

            ~DrawController();
            
            void ClearDrawing();
            bool IsDrawingLine();
            bool IsDrawingRect();
            bool IsDrawingPolygon();

            void drawGeometry();
            
            const Fl_Menu_Item* GetLineWidthChoiceItems();
            const DrawModel& GetModel();

            const Figure2f* GetPreviewFigure() const;

            // contoller management functions
            static DrawController* CreateInstance();
            void SetUIController(UIController* controller);

            void SetCurrentColor(Fl_Color color);
            Fl_Color GetCurrentColor() const;

            void SetCurrentLineWidth(LineWidth width);
            LineWidth GetCurrentLineWidth() const;

            // mouse event handlers
            bool OnMouseLeftDown(float scaledX, float scaledY);
            bool OnMouseLeftUp(float scaledX, float scaledY);
            bool OnMouseRightDown(float scaledX, float scaledY);
            bool OnMouseRightUp(float scaledX, float scaledY);
            bool OnMouseMove(float scaledX, float scaledY);

            // widget event static handlers
            static void OnDrawLine(Fl_Widget* sender, void* pUserData);
            static void OnDrawRect(Fl_Widget* sender, void* pUserData);
            static void OnDrawPolygon(Fl_Widget* sender, void* pUserData);
            static void OnDrawClear(Fl_Widget* sender, void* pUserData);
            static void OnDrawUndo(Fl_Widget* sender, void* pUserData);
            static void OnChooseColor(Fl_Widget* sender, void* pUserData);
            static void OnChangeLineWidth(Fl_Widget* sender, void* pUserData);
            static void OnLoadGeometry(Fl_Widget* sender, void* pUserData);
            static void OnSaveGeometry(Fl_Widget* sender, void* pUserData);
    };
}

#endif

