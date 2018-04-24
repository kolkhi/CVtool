#ifndef plot_draw_h
#define plot_draw_h

#include <mgl2/wnd.h>

namespace cvtool
{
    class PlotDraw : public mglDraw
    { 
    public:
        PlotDraw();
        virtual ~PlotDraw();
        int Draw(mglGraph *gr);       
    };
}

#endif