#include <QtGui/QApplication>
#include "MoulKI.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MoulKI w;
    w.show();
    return a.exec();
}
