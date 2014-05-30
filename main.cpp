// define DTV or UNITEST or WPS or MW or DTVWIN
// to call different entry point
#define MW

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
        //return 0;
    }

#else
#ifdef DTV
    #include "viewer.h"
    #include <QApplication>
    int main(int argc, char** argv)
    {
      QApplication application(argc,argv);
      DViewer::Viewer viewer;
      viewer.setWindowTitle("Delaunay Triangulation Viewer");
      viewer.show();
      return application.exec();
    }
#else
#ifdef DTVWIN
    #include "dviewerwindow.h"
    #include <QApplication>
    int main(int argc, char** argv)
    {
      QApplication application(argc,argv);
      DViewerWindow dvwin;
      dvwin.setWindowTitle("Delaunay Triangulation Viewer");
      dvwin.show();
      return application.exec();
    }
#endif
#endif
#endif
#endif

#include "../Tests/DelaunayTest.cpp"
