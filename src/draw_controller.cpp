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
    , drawingMode(DrawingMode::DM_None)
    , mouseClickCount(0)
    , previewFigure(nullptr)
{
    
}

DrawController::~DrawController()
{
    model.Clear();
    if(previewFigure)
        delete previewFigure;
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

void DrawController::EnableLineDrawingMode()
{
    drawingMode = DrawingMode::DM_Line;
    ResetDrawingParameters();
    uiController->UpdateDrawingButtons();
}

/*static*/ void DrawController::OnDrawLine(Fl_Widget* sender, void* pUserData)
{
    assert(pUserData);
    DrawController* controller = static_cast<DrawController*>(pUserData); 
    if(!controller)
    {
        return;
    }

    controller->EnableLineDrawingMode();
}

void DrawController::EnableRectDrawingMode()
{
    drawingMode = DrawingMode::DM_Rect;
    ResetDrawingParameters();
    uiController->UpdateDrawingButtons();
}

/*static*/ void DrawController::OnDrawRect(Fl_Widget* sender, void* pUserData)
{
    assert(pUserData);
    DrawController* controller = static_cast<DrawController*>(pUserData); 
    if(!controller)
    {
        return;
    }

    controller->EnableRectDrawingMode();
}

void DrawController::ClearDrawing()
{
    DisableDrawing();
    model.Clear();
    uiController->UpdateDrawingButtons();
    Redraw();
}

void DrawController::DisableDrawing()
{
    drawingMode = DrawingMode::DM_None;
    ResetDrawingParameters();
}

void DrawController::ResetDrawingParameters()
{
    mouseClickCount = 0;
    if(previewFigure)
        delete previewFigure;

    switch(drawingMode)
    {
        case DrawingMode::DM_Line:
            previewFigure = new Line2f(); 
            break;
        case DrawingMode::DM_Rect:
            previewFigure = new Rect2f();
            break;
        default:
            previewFigure = nullptr;
            break;
    }
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

bool DrawController::OnMouseDown(float scaledX, float scaledY)
{
    switch(drawingMode)
    {
        case DrawingMode::DM_Line:
        case DrawingMode::DM_Rect:
            mouseClickCount++;
            break;
        default:
            break;
    }
    return false;
}

bool DrawController::OnMouseUp(float scaledX, float scaledY)
{
    bool res = true;
    switch(drawingMode)
    {
        case DrawingMode::DM_Line:
            {
                if(mouseClickCount == 1)
                {
                    Line2f* prevLine = dynamic_cast<Line2f*>(previewFigure);
                    assert(prevLine);

                    prevLine->p1 = Point2f(scaledX, scaledY);
                }
                else if (mouseClickCount == 2)
                {
                    Line2f* prevLine = dynamic_cast<Line2f*>(previewFigure);
                    assert(prevLine);

                    prevLine->p2 = Point2f(scaledX, scaledY);
                    prevLine->color = GetCurrentColor();  
                    prevLine->lineWidth = GetCurrentLineWidth(); 
                    model.AddLine(*prevLine);
                    mouseClickCount = 0;
                }
            }
            break;
        case DrawingMode::DM_Rect:
            {
                if(mouseClickCount == 1)
                {
                    Rect2f* prevRect = dynamic_cast<Rect2f*>(previewFigure);
                    assert(prevRect);

                    prevRect->topLeft = Point2f(scaledX, scaledY);        
                }
                else if (mouseClickCount == 2)
                {
                    Rect2f* prevRect = dynamic_cast<Rect2f*>(previewFigure);
                    assert(prevRect);

                    prevRect->bottomRight = Point2f(scaledX, scaledY);
                    prevRect->lineWidth = GetCurrentLineWidth();
                    prevRect->color = GetCurrentColor(); 
                    model.AddRect(*prevRect);
                    mouseClickCount = 0;            
                }
            }
            break;
        default:
            res = false;
            break;
    }

    return res;
}

bool DrawController::OnMouseMove(float scaledX, float scaledY)
{
    bool res = true;
    switch(drawingMode)
    {
        case DrawingMode::DM_Line:
            {
                if (mouseClickCount == 1)
                {
                    Line2f* prevLine = dynamic_cast<Line2f*>(previewFigure);
                    assert(prevLine);

                    prevLine->p2 = Point2f(scaledX, scaledY); 
                    prevLine->color = GetCurrentColor();  
                    prevLine->lineWidth = GetCurrentLineWidth();
                }
            }
            break;
        case DrawingMode::DM_Rect:
            {
                if (mouseClickCount == 1)
                {
                    Rect2f* prevRect = dynamic_cast<Rect2f*>(previewFigure);
                    assert(prevRect);

                    prevRect->bottomRight = Point2f(scaledX, scaledY);  
                    prevRect->lineWidth = GetCurrentLineWidth();
                    prevRect->color = GetCurrentColor();
                }
            }
            break;
        default:
            res = false;
            break;
    }

    return res;
}

bool DrawController::IsDrawingLine()
{
    return (drawingMode == DrawingMode::DM_Line);
}

bool DrawController::IsDrawingRect()
{
    return (drawingMode == DrawingMode::DM_Rect);
}

const Figure2f* DrawController::GetPreviewFigure() const
{
    return ((mouseClickCount == 1) ? previewFigure : nullptr);
}
            
void DrawController::drawGeometry()
{
    drawLines();
    drawRectangles();
    drawPreview();
}

void DrawController::drawLine(const Line2f* line)
{
    if(!line)
        return;

    GLfloat lineWidth = static_cast<GLfloat>(static_cast<int>(line->lineWidth));
    uchar r,g,b;
    Fl::get_color(line->color,r,g,b);

    glLineWidth(lineWidth);
    glColor3f(r / 255.f, g / 255.f, b / 255.f);

    float p1_x = line->p1.x;
    float p1_y = line->p1.y;

    float p2_x = line->p2.x;
    float p2_y = line->p2.y;

    glBegin(GL_LINES);         
        glVertex2f(p1_x, p1_y); 
        glVertex2f(p2_x, p2_y);
    glEnd();
}

void DrawController::drawLines()
{
    if(model.GetLinesCount() == 0)
        return;

    for(auto i=0; i<model.GetLinesCount(); i++)
    {
        const Line2f* line = model.GetLine(i);
        drawLine(line);
    }
}

void DrawController::drawRect(const Rect2f* rect)
{
    if(!rect)
        return;

    GLfloat lineWidth = static_cast<GLfloat>(static_cast<int>(rect->lineWidth));
    uchar r,g,b;
    Fl::get_color(rect->color,r,g,b);

    glLineWidth(lineWidth);
    glColor3f(r / 255.f, g / 255.f, b / 255.f);

    float left = rect->topLeft.x;
    float right = rect->bottomRight.x;
    float top = rect->topLeft.y;
    float bottom = rect->bottomRight.y;

    glBegin(GL_LINE_LOOP);
        glVertex2f(left, top); 
        glVertex2f(right, top); 
        glVertex2f(right, bottom); 
        glVertex2f(left, bottom); 
    glEnd();
}

void DrawController::drawRectangles()
{
    if(model.GetRectanglesCount() == 0)
        return;

    for(auto i=0; i<model.GetRectanglesCount(); i++)
    {
        const Rect2f* rect = model.GetRect(i);
        drawRect(rect);
    }
}

void DrawController::drawPreview()
{
    auto figure = GetPreviewFigure();
    auto line = dynamic_cast<const Line2f*>(figure);
    auto rect = dynamic_cast<const Rect2f*>(figure);
    
    if(line)
    {
        drawLine(line);
    }
    else if(rect)
    {
        drawRect(rect);
    }
}