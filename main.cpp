#include <QtGui/QApplication>
#include <Debug/plDebug.h>
#include "MoulKI.h"

int main(int argc, char *argv[])
{
    plDebug::Init(plDebug::kDLAll);

    QApplication a(argc, argv);
    MoulKI w;
    w.show();
    return a.exec();
}
