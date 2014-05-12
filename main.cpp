#define UNITTEST 1

#include "mainwindow.h"
#include <QApplication>

#if UNITTEST==0
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    return a.exec();
}
#endif
