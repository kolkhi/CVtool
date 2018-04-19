#include <draw_controller.h>
#include <cassert>
#include <ui_controller.h>
#include <Fl/Fl_Color_Chooser.H>
#include <Fl/Fl_Button.H>

using namespace std;
using namespace cvtool;

static std::vector<Fl_Menu_Item> menuItems = {
  {"1",0,nullptr},
  {"2",0,nullptr},
  {"3",0,nullptr},
  {"4",0,nullptr},
  {"5",0,nullptr},
  {"6",0,nullptr},
  {"7",0,nullptr},
  {0}
};

DrawController::DrawController()
    : uiController(nullptr)
{
    
}

DrawController::~DrawController()
{
    model.Clear();
}

const DrawModel& DrawController::GetModel()
{
    return model;
}

void DrawController::Redraw()
{
    uiController->UpdateDrawing();
}

/*static*/ DrawController* DrawController::CreateInstance()
{
    return new DrawController();
}

void DrawController::SetCurrentColor(Fl_Color color)
{
    model.SetColor(color);
    Redraw();
}

Fl_Color DrawController::GetCurrentColor() const
{
    return model.GetColor();
}

void DrawController::SetCurrentLineWidth(LineWidth width)
{
    model.SetLineWidth(width);
    Redraw();
}

LineWidth DrawController::GetCurrentLineWidth() const
{
    return model.GetLineWidth();
}

void DrawController::SetUIController(UIController* controller)
{
    uiController = controller;
}

std::string DrawController::GetLineWidthString(LineWidth lineWidth)
{
    std::string lineWidthStr = std::to_string(static_cast<int>(lineWidth)); 
    return lineWidthStr;
}

const Fl_Menu_Item* DrawController::GetLineWidthChoiceItems()
{
    return menuItems.data();
}

/*static*/ void DrawController::OnDrawLine(Fl_Widget* sender, void* pUserData)
{
    assert(pUserData);
    DrawController* controller = static_cast<DrawController*>(pUserData); 
    if(!controller)
    {
        return;
    }
}

/*static*/ void DrawController::OnDrawRect(Fl_Widget* sender, void* pUserData)
{
    assert(pUserData);
    DrawController* controller = static_cast<DrawController*>(pUserData); 
    if(!controller)
    {
        return;
    }
}

void DrawController::ClearDrawing()
{
    model.Clear();
    Redraw();
}

/*static*/ void DrawController::OnDrawClear(Fl_Widget* sender, void* pUserData)
{
    assert(pUserData);
    DrawController* controller = static_cast<DrawController*>(pUserData); 
    if(!controller)
    {
        return;
    }

    controller->ClearDrawing();
}

bool DrawController::ChooseNewColor()
{
    Fl_Color c = GetCurrentColor();
    uchar r,g,b;
    Fl::get_color(c,r,g,b);
    if (!fl_color_chooser("Choose color",r,g,b,1)) 
        return false;
    
    c = fl_rgb_color(r,g,b);
    SetCurrentColor(c);
    assert(c == GetCurrentColor());
    
    return true;
}

/*static*/ void DrawController::OnChooseColor(Fl_Widget* sender, void* pUserData)
{
    assert(pUserData);
    DrawController* controller = static_cast<DrawController*>(pUserData); 
    if(!controller)
    {
        return;
    }

    if(!controller->ChooseNewColor())
        return;

    Fl_Button* btn = static_cast<Fl_Button*>(sender);
    assert(btn);
    if(!btn)
    {
        return;
    }

    btn->color(controller->GetCurrentColor());
    btn->redraw();
}

void DrawController::ChangeLineWidth(int index)
{
    LineWidth width = static_cast<LineWidth>(index + 1);
    SetCurrentLineWidth(width);
    assert(width == GetCurrentLineWidth());
}

/*static*/ void DrawController::OnChangeLineWidth(Fl_Widget* sender, void* pUserData)
{
    assert(pUserData);
    DrawController* controller = static_cast<DrawController*>(pUserData); 
    if(!controller)
    {
        return;
    }

    Fl_Choice* choice = static_cast<Fl_Choice*>(sender);
    assert(choice);
    if(!choice)
    {
        return;
    }

    controller->ChangeLineWidth(choice->value());
}
