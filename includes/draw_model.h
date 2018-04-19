#ifndef draw_model_h
#define draw_model_h

#include <Fl/Fl.H>
#include <string>
#include <vector>
#include <data_structures.h>

namespace cvtool
{
    class DrawModel 
    {
        private:
            // class data 
            LineWidth currentLineWidth;
            Fl_Color currentColor;

            std::vector<Line2f> linesCollection;
            std::vector<Rect2f> rectsCollection;
    
        public:
            DrawModel();
            ~DrawModel();

            void Clear();

            const Line2f* GetLine(int index) const; 
            int AddLine(const Point2f& p1, const Point2f& p2);
            int AddLine(const Line2f& line);

            const Rect2f* GetRect(int index) const;
            int AddRect(const Point2f& topLeft, const Point2f& bottomRight);
            int AddRect(const Rect2f& rect);

            void SetLineWidth(LineWidth lineWidth);
            LineWidth GetLineWidth() const;

            void SetColor(Fl_Color color);
            Fl_Color GetColor() const;

            int GetRectanglesCount() const;
            int GetLinesCount() const;
    };
}

#endif