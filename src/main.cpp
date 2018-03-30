#include <ui_controller.h>

using namespace std;
using namespace cvtool;

int main(int  argc,	char *argv[])
{
    UIController::SetupSystemSettings();
    return UIController::RunApplication(argc, argv);
}

