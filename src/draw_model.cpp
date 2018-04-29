#include <draw_model.h>
#include <algorithm>
#include <cmath>

using namespace cvtool;


DrawModel::DrawModel()
    : currentLineWidth(LineWidth::w1)
    , currentColor(FL_BLACK)
{
    
} 
    
DrawModel::~DrawModel()
{
    
}

void DrawModel::Clear()
{
    linesCollection.clear();
    rectsCollection.clear(); 
    polysCollection.clear();                  
}

const Line2f* DrawModel::GetLine(int index) const
{
    if(index < 0 || index >= static_cast<int>(linesCollection.size()))
        return nullptr;

    return &linesCollection[index];
} 

int DrawModel::AddLine(const Point2f& p1, const Point2f& p2)
{
    Line2f newLine;
    newLine.p1 = p1;
    newLine.p2 = p2;
    return AddLine(newLine);
}

int DrawModel::AddLine(const Line2f& line)
{
    linesCollection.push_back(line);
    return (static_cast<int>(linesCollection.size()) - 1);
}

const Rect2f* DrawModel::GetRect(int index) const
{
    if(index < 0 || index >= static_cast<int>(rectsCollection.size()))
        return nullptr;

    return &rectsCollection[index];
}

int DrawModel::AddRect(const Point2f& topLeft, const Point2f& bottomRight)
{
    Rect2f rect;
    rect.topLeft = topLeft;
    rect.bottomRight = rect.bottomRight;
    return AddRect(rect); 
}

int DrawModel::AddRect(const Rect2f& rect)
{
    rectsCollection.push_back(rect);
    return (static_cast<int>(rectsCollection.size()) - 1);
}

const Polygon2f* DrawModel::GetPolygon(int index) const
{
    if(index < 0 || index >= static_cast<int>(polysCollection.size()))
        return nullptr;

    return &polysCollection[index];
}

int DrawModel::AddPolygon(const std::vector<Point2f>& points)
{
    Polygon2f polygon;
    polygon.points.insert(polygon.points.begin(), points.begin(), points.end());
    return AddPolygon(polygon);
}

int DrawModel::AddPolygon(const Polygon2f& poly)
{
    polysCollection.push_back(poly);
    return (static_cast<int>(polysCollection.size()) - 1);
}

void DrawModel::SetLineWidth(LineWidth lineWidth)
{
    currentLineWidth = lineWidth;
}

LineWidth DrawModel::GetLineWidth() const
{
    return currentLineWidth;
}

void DrawModel::SetColor(Fl_Color color)
{
    currentColor = color;
}

Fl_Color DrawModel::GetColor() const
{
    return currentColor;
}

int DrawModel::GetRectanglesCount() const
{
    return static_cast<int>(rectsCollection.size());
}

int DrawModel::GetLinesCount() const
{
    return static_cast<int>(linesCollection.size());
}

int DrawModel::GetPolygonsCount() const
{
    return static_cast<int>(polysCollection.size());
}