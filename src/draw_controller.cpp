#include <draw_controller.h>
#include <resources.h>
#include <cassert>
#include <ui_controller.h>
#include <Fl/Fl_Color_Chooser.H>
#include <Fl/Fl_Button.H>
#include <FL/Fl_Native_File_Chooser.H>
#include <json/json.h>
#include <fstream>

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
    drawingMode = (drawingMode != DrawingMode::DM_Line) ? DrawingMode::DM_Line : DrawingMode::DM_None;
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
    drawingMode = (drawingMode != DrawingMode::DM_Rect) ? DrawingMode::DM_Rect : DrawingMode::DM_None;
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

void DrawController::EnablePolygonDrawingMode()
{
    drawingMode = (drawingMode != DrawingMode::DM_Polygon) ? DrawingMode::DM_Polygon : DrawingMode::DM_None;
    ResetDrawingParameters();
    uiController->UpdateDrawingButtons();
}

/*static*/ void DrawController::OnDrawPolygon(Fl_Widget* sender, void* pUserData)
{
    assert(pUserData);
    DrawController* controller = static_cast<DrawController*>(pUserData); 
    if(!controller)
    {
        return;
    }

    controller->EnablePolygonDrawingMode();
}

void DrawController::ClearDrawing()
{
    AddToUndo();

    DisableDrawing();
    model.Clear();
    uiController->UpdateDrawingButtons();
    Redraw();
}

void DrawController::RemoveLastAddedFigure()
{
    if(Undo())
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
        case DrawingMode::DM_Polygon:
            previewFigure = new Polygon2f();
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

/*static*/ void DrawController::OnDrawUndo(Fl_Widget* sender, void* pUserData)
{
    assert(pUserData);
    DrawController* controller = static_cast<DrawController*>(pUserData); 
    if(!controller)
    {
        return;
    }

    controller->RemoveLastAddedFigure();
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

bool DrawController::OnMouseLeftDown(float scaledX, float scaledY)
{
    switch(drawingMode)
    {
        case DrawingMode::DM_Line:
        case DrawingMode::DM_Rect:
        case DrawingMode::DM_Polygon:
            mouseClickCount++;
            break;
        default:
            break;
    }
    return false;
}

bool DrawController::OnMouseRightDown(float scaledX, float scaledY)
{
    return false;
}

bool DrawController::OnMouseLeftUp(float scaledX, float scaledY)
{
    bool res = true;
    switch(drawingMode)
    {
        case DrawingMode::DM_Line:
            {
                Line2f* prevLine = dynamic_cast<Line2f*>(previewFigure);
                assert(prevLine);
                if(mouseClickCount == 1)
                {
                    prevLine->p1 = Point2f(scaledX, scaledY);
                }
                else if (mouseClickCount == 2)
                {
                    AddToUndo();
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
                Rect2f* prevRect = dynamic_cast<Rect2f*>(previewFigure);
                assert(prevRect);
                if(mouseClickCount == 1)
                {
                    prevRect->topLeft = Point2f(scaledX, scaledY);        
                }
                else if (mouseClickCount == 2)
                {
                    AddToUndo();
                    prevRect->bottomRight = Point2f(scaledX, scaledY);
                    prevRect->lineWidth = GetCurrentLineWidth();
                    prevRect->color = GetCurrentColor(); 
                    model.AddRect(*prevRect);
                    mouseClickCount = 0;            
                }
            }
            break;
        case DrawingMode::DM_Polygon:
            {
                Polygon2f* prevPolygon = dynamic_cast<Polygon2f*>(previewFigure);
                assert(prevPolygon);
                if(mouseClickCount == 1)
                {
                    prevPolygon->points.push_back(Point2f(scaledX, scaledY));
                }
                else
                {   
                    prevPolygon->points[(int)prevPolygon->points.size()-1] = Point2f(scaledX, scaledY);
                }

                //add preview point
                prevPolygon->points.push_back(Point2f(scaledX, scaledY));
            }
            break;
        default:
            res = false;
            break;
    }

    return res;
}

bool DrawController::OnMouseRightUp(float scaledX, float scaledY)
{
    bool res = true;
    switch(drawingMode)
    {
        case DrawingMode::DM_Polygon:
            {
                AddToUndo();
                Polygon2f* prevPolygon = dynamic_cast<Polygon2f*>(previewFigure);
                assert(prevPolygon);

                prevPolygon->lineWidth = GetCurrentLineWidth();
                prevPolygon->color = GetCurrentColor(); 

                if((int)prevPolygon->points.size() > mouseClickCount)
                    prevPolygon->points.resize(mouseClickCount);

                model.AddPolygon(*prevPolygon);
                mouseClickCount = 0;  
                prevPolygon->points.clear();        
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
        case DrawingMode::DM_Polygon:
            {
                if (mouseClickCount >= 1)
                {
                    Polygon2f* prevPolygon = dynamic_cast<Polygon2f*>(previewFigure);
                    assert(prevPolygon);

                    prevPolygon->points[(int)prevPolygon->points.size()-1] = Point2f(scaledX, scaledY); 
                    
                    prevPolygon->lineWidth = GetCurrentLineWidth();
                    prevPolygon->color = GetCurrentColor();
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

bool DrawController::IsDrawingPolygon()
{
    return (drawingMode == DrawingMode::DM_Polygon);
}

const Figure2f* DrawController::GetPreviewFigure() const
{
    return previewFigure;
}
            
void DrawController::drawGeometry()
{
    drawLines();
    drawRectangles();
    drawPolygons();
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

void DrawController::drawPolygon(const Polygon2f* poly)
{
    if(!poly || poly->points.size() < 2)
        return;

    GLfloat lineWidth = static_cast<GLfloat>(static_cast<int>(poly->lineWidth));
    uchar r,g,b;
    Fl::get_color(poly->color,r,g,b);

    glLineWidth(lineWidth);
    glColor3f(r / 255.f, g / 255.f, b / 255.f);

    glBegin(GL_LINE_LOOP);
    for(auto i=0; i<(int)poly->points.size(); i++)
    {
        glVertex2f(poly->points[i].x, poly->points[i].y);
    }
    glEnd();
}

void DrawController::drawPolygons()
{
    if(model.GetPolygonsCount() == 0)
        return;

    for(auto i=0; i<model.GetPolygonsCount(); i++)
    {
        const Polygon2f* poly = model.GetPolygon(i);
        drawPolygon(poly);
    }
}

void DrawController::drawPreview()
{
    if(mouseClickCount == 0)
        return;
        
    auto figure = GetPreviewFigure();
    auto line = dynamic_cast<const Line2f*>(figure);
    auto rect = dynamic_cast<const Rect2f*>(figure);
    auto poly = dynamic_cast<const Polygon2f*>(figure);
    
    if(line)
    {
        drawLine(line);
    }
    else if(rect)
    {
        drawRect(rect);
    }
    else if(poly)
    {
        drawPolygon(poly);
    }
}

void DrawController::AddToUndo()
{
    if((int)undoCollection.size() == UNDO_LIMIT)
        undoCollection.pop_front();
        
    undoCollection.push_back(model);
}

bool DrawController::Undo()
{
    if(undoCollection.size() == 0)
        return false;

    model = undoCollection.back();
    undoCollection.pop_back();
    return true;
}

bool DrawController::ParseLine(const Json::Value& value, Line2f& line)
{
    auto width = value["width"];
    if(width.isNumeric())
        line.lineWidth = static_cast<LineWidth>(width.asInt());
    else
        return false;

    auto color = value["color"];
    if(color.isNumeric())
        line.color = static_cast<Fl_Color>(color.asUInt());
    else
        return false;

    auto p1_x = value["p1"]["x"];
    if(p1_x.isNumeric()) 
        line.p1.x = p1_x.asFloat();
    else
        return false;

    auto p1_y = value["p1"]["y"];
    if(p1_y.isNumeric()) 
        line.p1.y = p1_y.asFloat();
    else
        return false;

    auto p2_x = value["p2"]["x"];
    if(p2_x.isNumeric()) 
        line.p2.x = p2_x.asFloat();
    else
        return false;

    auto p2_y = value["p2"]["y"];
    if(p2_y.isNumeric()) 
        line.p2.y = p2_y.asFloat();
    else
        return false;

    return true;
}

bool DrawController::ParseRect(const Json::Value& value, Rect2f& rect)
{
    auto width = value["width"];
    if(width.isNumeric())
        rect.lineWidth = static_cast<LineWidth>(width.asInt());
    else
        return false;

    auto color = value["color"];
    if(color.isNumeric())
        rect.color = static_cast<Fl_Color>(color.asUInt());
    else
        return false;

    auto tl_x = value["tl"]["x"];
    if(tl_x.isNumeric()) 
        rect.topLeft.x = tl_x.asFloat();
    else
        return false;

    auto tl_y = value["tl"]["y"];
    if(tl_y.isNumeric()) 
        rect.topLeft.y = tl_y.asFloat();
    else
        return false;

    auto br_x = value["br"]["x"];
    if(br_x.isNumeric()) 
        rect.bottomRight.x = br_x.asFloat();
    else
        return false;

    auto br_y = value["br"]["y"];
    if(br_y.isNumeric()) 
        rect.bottomRight.y = br_y.asFloat();
    else
        return false;

    return true;
}

bool DrawController::ParsePoly(const Json::Value& value, Polygon2f& poly)
{
    auto width = value["width"];
    if(width.isNumeric())
        poly.lineWidth = static_cast<LineWidth>(width.asInt());
    else
        return false;

    auto color = value["color"];
    if(color.isNumeric())
        poly.color = static_cast<Fl_Color>(color.asUInt());
    else
        return false;


    auto points = value["points"];
    if(!points.isNumeric())
        return false;

    int pointsCnt = points.asInt();
    Json::Value pointsArray = value["array"];
    for(auto i=0; i<pointsCnt; i++)
    {
        Json::Value pointsstr = pointsArray[i];
        if(!pointsstr.isObject())
            return false;
        
        Point2f pnt;
        auto x = pointsstr["point"]["x"];
        if(x.isNumeric())
            pnt.x = x.asFloat();
        else
            return false;
        
        auto y = pointsstr["point"]["y"];
        if(y.isNumeric())
            pnt.y = y.asFloat();
        else
            return false;
        
        poly.points.push_back(pnt);
    }

    return true;
}

void DrawController::ReadLatestVersion(const Json::Value& root)
{
    std::vector<Line2f> lines;
    Json::Value linesObject = root["lines"];
    if (linesObject.isObject()) 
    {
        Json::Value lineCnt = root["lines"]["count"];
        if(lineCnt.isNumeric())
        {
            int count = lineCnt.asInt();
            Json::Value array = root["lines"]["array"];
            for(auto i=0; i<count; i++)
            {
                const Json::Value& linestr = array[i];
                Line2f line;
                if(ParseLine(linestr["line"], line))
                    lines.push_back(line);
                else
                    throw std::runtime_error("Reading of the line object " + std::to_string(i+1) + " has failed.");
            }
        }
    }        

    std::vector<Rect2f> rects;
    Json::Value rectanglesObject = root["rectangles"];
    if (rectanglesObject.isObject()) 
    {
        Json::Value rectCnt = root["rectangles"]["count"];
        if(rectCnt.isNumeric())
        {
            int count = rectCnt.asInt();
            Json::Value array = root["rectangles"]["array"];
            for(auto i=0; i<count; i++)
            {
                const Json::Value& rectstr = array[i];
                Rect2f rect;
                if(ParseRect(rectstr["rect"], rect))
                    rects.push_back(rect);
                else
                    throw std::runtime_error("Reading of the rectangle object " + std::to_string(i+1) + " has failed.");
            }
        }
    }

    std::vector<Polygon2f> polys;
    Json::Value polylgonsObject = root["polygons"];
    if (polylgonsObject.isObject()) 
    {
        Json::Value polyCnt = root["polygons"]["count"];
        if(polyCnt.isNumeric())
        {
            int count = polyCnt.asInt();
            Json::Value array = root["polygons"]["array"];
            for(auto i=0; i<count; i++)
            {
                const Json::Value& polystr = array[i];
                Polygon2f poly;
                if(ParsePoly(polystr["poly"], poly))
                    polys.push_back(poly);
                else
                    throw std::runtime_error("Reading of the polygon object " + std::to_string(i+1) + " has failed.");
            }
        }
    }

    /*Fl_Color maincolor = model.GetColor();
    Json::Value cl = root["color"];
    if(cl.isNumeric())
    {
        maincolor = static_cast<Fl_Color>(cl.asUInt()); 
    }

    LineWidth lineWidth = model.GetLineWidth();
    Json::Value width = root["width"];
    if(width.isNumeric())
    {
        lineWidth = static_cast<LineWidth>(width.asInt());
    }*/

    AddToUndo();
    model.Clear();

    //model.SetColor(maincolor);
    //model.SetLineWidth(lineWidth);

    for(auto& item: lines)
    {
        model.AddLine(item);
    }

    for(auto& item: rects)
    {
        model.AddRect(item);
    }

    for(auto& item: polys)
    {
        model.AddPolygon(item);
    }

    Redraw();
}

void DrawController::LoadGeometryFromFile(const std::string& filePath)
{
    try
    {
        std::ifstream file(filePath);
        Json::CharReaderBuilder rbuilder;
        std::string errs;
        Json::Value root;
        bool ok = Json::parseFromStream(rbuilder, file, &root, &errs);
        if(!ok)
            throw std::runtime_error("File loading failed: " + errs);

        Json::Value appID = root["appid"];
        if(!appID.isString())
        {
            throw std::runtime_error("Incorrect file format."); 
        }

        std::string appIDstr = appID.asString();
        if(appIDstr != APP_ID)
        {
            throw std::runtime_error("Unknown application ID.");
        }


        Json::Value version = root["version"]; 
        if(!version.isNumeric())
        {
            throw std::runtime_error("Unknown file version.");     
        }

        int loaded_version = version.asInt();
        switch(loaded_version)
        {
            case FILE_VERSION:
                ReadLatestVersion(root);
                break;
            default:
                {
                    std::string msg = "File version " + std::to_string(loaded_version) + " is not supported";
                    throw std::runtime_error(msg);
                }  
        }
    }
    catch(exception& ex)
    {
        fprintf(stderr, "ERROR: %s\n", ex.what()); 
        fl_message("ERROR: %s\n", ex.what());
    }
}

/*static*/ void DrawController::OnLoadGeometry(Fl_Widget* sender, void* pUserData)
{
    assert(pUserData);
    DrawController* controller = static_cast<DrawController*>(pUserData); 
    if(!controller)
    {
        return;
    }

    // Create native chooser
    Fl_Native_File_Chooser native;
    native.title("Open geometry file");
    native.type(Fl_Native_File_Chooser::BROWSE_FILE);
    native.filter(geomFileFilter);
    
    // Show native chooser
    switch ( native.show() ) 
    {
        case -1: 
            fprintf(stderr, "ERROR: %s\n", native.errmsg()); 
            fl_message("ERROR: %s\n", native.errmsg());
        break;	// ERROR
        case  1: 
            fprintf(stderr, "*** CANCEL\n"); 
            fl_beep(); 
        break;		// CANCEL
        default: 								// PICKED FILE
            {
                if ( native.filename() ) 
                {
                    controller->LoadGeometryFromFile(native.filename());
                } 
            }
        break;
    }
}

void DrawController::SaveGeometryToFile(const std::string& filePath)
{
    try
    {
        Json::Value root;
        root["appid"] = APP_ID;
        root["version"] = FILE_VERSION; 

        //root["color"] = model.GetColor();
        //root["width"] = static_cast<int>(model.GetLineWidth());

        root["lines"]["count"] = model.GetLinesCount();
        for(auto i=0; i<model.GetLinesCount(); i++)
        {
            const Line2f* line = model.GetLine(i);
            Json::Value arraystr;
            arraystr["line"]["index"] = i + 1;
            arraystr["line"]["color"] = line->color;
            arraystr["line"]["width"] = static_cast<int>(line->lineWidth);
            arraystr["line"]["p1"]["x"] = line->p1.x;
            arraystr["line"]["p1"]["y"] = line->p1.y;
            arraystr["line"]["p2"]["x"] = line->p2.x;
            arraystr["line"]["p2"]["y"] = line->p2.y;
            
            root["lines"]["array"].append(arraystr);
        }

        root["rectangles"]["count"] = model.GetRectanglesCount();
        for(auto i=0; i<model.GetRectanglesCount(); i++)
        {
            const Rect2f* rect = model.GetRect(i);
            Json::Value arraystr;
            arraystr["rect"]["index"] = i + 1;
            arraystr["rect"]["color"] = rect->color;
            arraystr["rect"]["width"] = static_cast<int>(rect->lineWidth);
            arraystr["rect"]["tl"]["x"] = rect->topLeft.x;
            arraystr["rect"]["tl"]["y"] = rect->topLeft.y;
            arraystr["rect"]["br"]["x"] = rect->bottomRight.x;
            arraystr["rect"]["br"]["y"] = rect->bottomRight.y;
            
            root["rectangles"]["array"].append(arraystr);
        }

        root["polygons"]["count"] = model.GetPolygonsCount();
        for(auto i=0; i<model.GetPolygonsCount(); i++)
        {
            const Polygon2f* poly = model.GetPolygon(i);
            Json::Value arraystr;
            arraystr["poly"]["index"] = i + 1;
            arraystr["poly"]["color"] = poly->color;
            arraystr["poly"]["width"] = static_cast<int>(poly->lineWidth);
            arraystr["poly"]["points"] = static_cast<int>(poly->points.size());
            for(auto j=0; j<(int)poly->points.size(); j++)
            {
                Json::Value pointsstr;
                pointsstr["point"]["x"] = poly->points[j].x;
                pointsstr["point"]["y"] = poly->points[j].y;
                arraystr["poly"]["array"].append(pointsstr); 
            }
            
            root["polygons"]["array"].append(arraystr);
        }

        Json::StreamWriterBuilder builder;
        std::string document = Json::writeString(builder, root);

        std::ofstream file(filePath);
        file << document << std::endl;  // add lf and flush
        file.close();
    }
    catch(exception& ex)
    {
        fprintf(stderr, "ERROR: %s\n", ex.what()); 
        fl_message("ERROR: %s\n", ex.what());
    }
}

/*static*/ void DrawController::OnSaveGeometry(Fl_Widget* sender, void* pUserData)
{
    assert(pUserData);
    DrawController* controller = static_cast<DrawController*>(pUserData); 
    if(!controller)
    {
        return;
    }

   // Create native chooser
    Fl_Native_File_Chooser native;
    native.title("Save geometry file");
    native.type(Fl_Native_File_Chooser::BROWSE_SAVE_FILE);
    native.filter(geomFileFilter);
    native.preset_file("figures.cvgeo");
    // Show native chooser
    switch ( native.show() ) 
    {
        case -1: 
            fprintf(stderr, "ERROR: %s\n", native.errmsg()); 
            fl_message("ERROR: %s\n", native.errmsg());
        break;	// ERROR
        case  1: 
            fprintf(stderr, "*** CANCEL\n"); 
            fl_beep(); 
        break;		// CANCEL
        default: 								// PICKED FILE
            {
                if ( native.filename() ) 
                {
                    controller->SaveGeometryToFile(native.filename());
                } 
            }
        break;
    }
}