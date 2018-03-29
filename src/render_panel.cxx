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

#include "render_panel.h"
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <FL/filename.H>
#include <FL/fl_ask.H>
#include <FL/Fl_Shared_Image.H>
#include <FL/Fl_Preferences.H>
#if defined(WIN32) && !defined(__CYGWIN__)
#include <io.h>
#else
#include <unistd.h>
#endif // WIN32 && !__CYGWIN__
extern Fl_Preferences fluid_prefs;

using namespace cvtool;

/**
 Window for rendering
*/

RenderWnd *render_panel=(RenderWnd *)0;

RenderWnd::RenderWnd(int W, int H, const char* l) :    
            Fl_Gl_Window(W, H, l)//, mpFrame(nullptr)
{
    mode(FL_DOUBLE);
    resizable(this);
}

/*
void RenderWnd::updateFrame(const UAVV_IMAGE buf)
{
    // Keep a reference to the frame buffer
    if (!buf) 
      return;
    
    uavv_image_destroy(mpFrame);
    mpFrame = uavv_image_copy(buf);

    // Schedule a redraw
    redraw();
}
*/

void RenderWnd::draw()
{
    /*if (!valid())  
      ortho();

    // Update GL frame
    if (mpFrame)
    {
        mGlFrame.copy(mpFrame);
        uavv_image_destroy(mpFrame);
        mpFrame = NULL;
    }

    glClearColor(0, 0, 0, 0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    if (!mGlFrame.isValid())  return;

    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_BLEND);
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);

    // Fit GL frame to window
    float scale_x = w() / (float)mGlFrame.width();
    float scale_y = h() / (float)mGlFrame.height();
    glPushMatrix();
    glScalef(scale_x, scale_y, 1.0);
    mGlFrame.draw();
    glPopMatrix();

    glDisable(GL_BLEND);*/
}

/*static*/ RenderWnd* RenderWnd::make_video_panel(int W, int H, const char* l) 
{
  RenderWnd* w;
  { 
    RenderWnd* o = new RenderWnd(W, H, l);
    w = o; if (w) {/* empty */}
    o->box(FL_FLAT_BOX);
    o->color(FL_BACKGROUND_COLOR);
    o->selection_color(FL_BACKGROUND_COLOR);
    o->labeltype(FL_NO_LABEL);
    o->labelfont(0);
    o->labelsize(14);
    o->labelcolor(FL_FOREGROUND_COLOR);
    o->align(Fl_Align(FL_ALIGN_TOP));
    o->when(FL_WHEN_RELEASE);
    o->set_non_modal();
    o->end();
    o->resizable(o);
  } // RenderWnd* o
  return w;
}

//
// End of "$Id: renderer_panel.fl 02 2018-03-29 18:21:11Z gsergia $".
//