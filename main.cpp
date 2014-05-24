// define DTV or UNITEST or WPS or MW
// to call different entry point
#define DTV

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
#ifdef DTV
    #include "../dviewer.h"
    #include "../dcamera.h"
    #include <QApplication>
    //#include <memory>

    int main(int argc, char** argv)
    {
      QApplication application(argc,argv);
      std::unique_ptr<DCamera> cam(new DCamera);
      DT::DViewer viewer(std::move(cam));
      viewer.setWindowTitle("Delaunay Triangulation Viewer");
      viewer.show();
      return application.exec();
    }
#endif
#endif
#endif

#include "../Tests/DelaunayTest.cpp"
