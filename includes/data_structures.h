#ifndef data_structures_h
#define data_structures_h

#include <string>
#include <vector>

namespace cvtool
{
    enum class ZoomState { ZoomOff = 0, ZoomIn = 1 };    
    enum class ZoomValue { x1 = 0, x2, x4, x8, x16, x32, x64, ZoomValueLast };
    enum class LineWidth { w1 = 1, w2 = 2, w3 = 3, w4 = 4, w5 = 5, w6 = 6, w7 = 7, wLast };
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

    struct Point2f
    {
        Point2f() : x(0.0), y(0.0) {}
        float x;
        float y;
    };

    struct Line2f
    {
        Line2f() {}
        Point2f p1;
        Point2f p2;
    };

    struct Rect2f
    {
        Rect2f(){}    
        Point2f topLeft;
        Point2f bottomRight;
    };
}

#endif