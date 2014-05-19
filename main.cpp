#define UNITTEST 0

#include "mainwindow.h"
#include <QApplication>

#if UNITTEST
#include <gtest/gtest.h>
int main(int argc, char *argv[])
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
#else
#include "triangulation.h"
//int main2(int argc, char *argv[])
//{
//    //QApplication a(argc, argv);
//    //MainWindow w;
//    //w.show();
//    //return a.exec();
//    return 0;
//}
#endif
