﻿// define DVIEWER or UNITEST or WPS or MW
// to call different entry point
#define DVIEWER

#ifdef UNITTEST
    #include <gtest/gtest.h>
    int main(int argc, char *argv[])
    {
        testing::InitGoogleTest(&argc, argv);
        return RUN_ALL_TESTS();
    }
#else
#ifdef MW
    #include "mainwindow.h"
    #include <QApplication>
    #include "triangulation.h"
    int main(int argc, char *argv[])
    {
        QApplication a(argc, argv);
        MainWindow w;
        w.show();
        return a.exec();
        return 0;
    }

#else
#ifdef DVIEWER
    #include "../viewer.h"
    #include <QApplication>
    int main(int argc, char** argv)
    {
      QApplication application(argc,argv);
      Viewer viewer;
      viewer.setWindowTitle("Delaunay Viewer");
      viewer.show();
      return application.exec();
    }
#endif
#endif
#endif
