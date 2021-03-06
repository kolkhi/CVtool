//
// "$Id: renderer_panel.fl 02 2018-03-29 18:21:11Z gsergia $"
//
// FLUID template support for the Fast Light Tool Kit (FLTK).
//
// Copyright 1998-2016 by Bill Spitzak and others.
//
// This library is free software. Distribution and use rights are outlined in
// the file "COPYING" which should have been included with this file.  If this
// file is missing or damaged, see the license at:
//
//     http://www.fltk.org/COPYING.php
//
// Please report all bugs and problems on the following page:
//
//     http://www.fltk.org/str.php
//

// generated by Fast Light User Interface Designer (fluid) version 1.0304

#ifndef renderer_panel_h
#define renderer_panel_h
#include <FL/Fl.H>

#include <stdio.h>
#include <stdlib.h>
#include <uavv_image.h>

#include <FL/Fl_Gl_Window.H>
#include <FL/Fl.H>
#include <FL/gl.h>
#include <mutex>
#include <gl_uavv_image.h>


/**
 Video renderer window
*/

namespace cvtool
{
    class UIController;
    class RenderWnd : public Fl_Gl_Window 
    {

    private:
        GLUavvImage mGlFrame;
        UAVV_IMAGE  mpFrame;
        std::mutex  imageMutex;
        UIController* pController; 
        
    protected:
        void draw();
        int handle(int event);
         
    public:
        RenderWnd(int W, int H, const char* l = 0);
        void UpdateGLFrame(const UAVV_IMAGE buf);
        void SetUIController(UIController* controller);
        UAVV_IMAGE GetCurrentFrameCopy();
        void CleanUp();
        void UpdateDrawing();
    };
}

#endif

//
// End of "$Id: renderer_panel.fl 02 2018-03-29 18:21:11Z gsergia $".
//
