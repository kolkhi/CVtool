#include <plot_drawer.h>

using namespace cvtool;

PlotDraw::PlotDraw() 
{
}

PlotDraw::~PlotDraw() 
{ 

}

// Handle drawing all cells in table
int PlotDraw::Draw(mglGraph *gr)
{
    gr->SubPlot(2,2,0); gr->Title("Axis origin, Grid"); gr->SetOrigin(0,0);
    gr->Axis(); gr->Grid(); gr->FPlot("x^3");
    gr->SubPlot(2,2,1); gr->Title("2 axis");
    gr->SetRanges(-1,1,-1,1); gr->SetOrigin(-1,-1,-1); // first axis
    gr->Axis(); gr->Label('y',"axis 1",0); gr->FPlot("sin(pi*x)");
    gr->SetRanges(0,1,0,1); gr->SetOrigin(1,1,1); // second axis
    gr->Axis(); gr->Label('y',"axis 2",0); gr->FPlot("cos(pi*x)");
    gr->SubPlot(2,2,3); gr->Title("More axis");
    gr->SetOrigin(NAN,NAN); gr->SetRange('x',-1,1);
    gr->Axis(); gr->Label('x',"x",0); gr->Label('y',"y_1",0);
    gr->FPlot("x^2","k");
    gr->SetRanges(-1,1,-1,1); gr->SetOrigin(-1.3,-1); // second axis
    gr->Axis("y","r"); gr->Label('y',"#r{y_2}",0.2);
    gr->FPlot("x^3","r");
    gr->SubPlot(2,2,2); gr->Title("4 segments, inverted axis");
    gr->SetOrigin(0,0);
    gr->InPlot(0.5,1,0.5,1); gr->SetRanges(0,10,0,2); gr->Axis();
    gr->FPlot("sqrt(x/2)"); gr->Label('x',"W",1); gr->Label('y',"U",1);
    gr->InPlot(0,0.5,0.5,1); gr->SetRanges(1,0,0,2); gr->Axis("x");
    gr->FPlot("sqrt(x)+x^3"); gr->Label('x',"\\tau",-1);
    gr->InPlot(0.5,1,0,0.5); gr->SetRanges(0,10,4,0); gr->Axis("y");
    gr->FPlot("x/4"); gr->Label('y',"L",-1);
    gr->InPlot(0,0.5,0,0.5); gr->SetRanges(1,0,4,0); gr->FPlot("4*x^2");

    //if(pWnd)
    //    pWnd->Update();
        
    return 0;
}