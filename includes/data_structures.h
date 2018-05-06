#ifndef data_structures_h
#define data_structures_h

#include <string>
#include <vector>
#include <Fl/Fl.H>
#include <uavv_image.h>

namespace cvtool
{
    enum class ZoomMode { ZoomOff = 0, ZoomIn = 1 };    
    enum class ZoomValue { x1 = 0, x2, x4, x8, x16, x32, x64, ZoomValueLast };
    enum class LineWidth { w1 = 1, w2 = 2, w3 = 3, w4 = 4, w5 = 5, w6 = 6, w7 = 7, wLast };
    enum class DrawingMode { DM_None = 0, DM_Line = 1, DM_Rect = 2, DM_Polygon = 3 };
    struct KLVItem
    {
        std::string itemName;
        std::string itemValue;
        bool itemState;

        std::string GetStateString()
        {
            return itemState ? "Error" : "OK";
        }
    }; 

    struct ThumbnailData
    {
        ThumbnailData() : img(nullptr){}
        
        UAVV_IMAGE img;
        std::string imgName;
    }; 

    struct Point2f
    {
        Point2f() : x(0.0), y(0.0) {}
        Point2f(float _x, float _y) : x(_x), y(_y) {}
        float x;
        float y;
    };

    class Figure2f
    {
        public:
            Figure2f() : lineWidth(LineWidth::w1), color(FL_BLACK)  {}
            virtual ~Figure2f() {}
            LineWidth lineWidth;
            Fl_Color color;
    };

    class Line2f : public Figure2f
    {
        public:
            Line2f() {}
            virtual ~Line2f() override {}
            Point2f p1;
            Point2f p2;
    };

    class Rect2f : public Figure2f
    {
        public:
            Rect2f(){}   
            virtual ~Rect2f() override {} 
            Point2f topLeft;
            Point2f bottomRight;
    };

    class Polygon2f : public Figure2f
    {
        public:
            Polygon2f() {}
            virtual ~Polygon2f() override {}
            std::vector<Point2f> points;
    };
}

#endif