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
            std::vector<Polygon2f> polysCollection;
            
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

            const Polygon2f* GetPolygon(int index) const;
            int AddPolygon(const std::vector<Point2f>& points);
            int AddPolygon(const Polygon2f& poly);

            void SetLineWidth(LineWidth lineWidth);
            LineWidth GetLineWidth() const;

            void SetColor(Fl_Color color);
            Fl_Color GetColor() const;

            int GetRectanglesCount() const;
            int GetLinesCount() const;
            int GetPolygonsCount() const;
    };
}

#endif