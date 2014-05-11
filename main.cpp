#define UNITTEST 1

#include "mainwindow.h"
#include <QApplication>
#include "Tests/MyFirstTest.cpp"

int main(int argc, char *argv[])
{

#if UNITTEST
    MyFirstTest test1;
    QTest::qExec(&test1);
#endif

    QApplication a(argc, argv);
    MainWindow w;
    w.show();

    return a.exec();
}
