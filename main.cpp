#include <QApplication>
#include <Debug/plDebug.h>
#include "MoulKI.h"

int main(int argc, char *argv[])
{
    plDebug::Init(plDebug::kDLAll);
    plResManager resmgr(PlasmaVer::pvMoul);
    plSDLMgr sdlmgr;

    QApplication a(argc, argv);
    MoulKI w(&resmgr, &sdlmgr);
    w.show();
    return a.exec();
}
