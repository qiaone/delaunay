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
int main(int argc, char *argv[])
{
    //QApplication a(argc, argv);
    //MainWindow w;
    //w.show();
    //return a.exec();

    using namespace Delaunay;
    Point p1(1, 2, 0);
    Point p2(100, 22, 0);
    Point p3(35, 27, 0);
    std::vector<Point> points;
    points.push_back(p1);
    points.push_back(p2);
    points.push_back(p3);
    Triangulation T(points);
    T.perform();
    return 0;
}
#endif
