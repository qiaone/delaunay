#-------------------------------------------------
#
# Project created by QtCreator 2014-05-09T11:39:58
#
#-------------------------------------------------

QT       += core gui opengl xml

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Delaunay
TEMPLATE = app

SOURCES += main.cpp\
    mainwindow.cpp \
    Tests/OpenMeshTest.cpp \
    dviewer.cpp \
    Tests/DelaunayTest.cpp \
    delaunay.cpp \
    randompointsdialog.cpp \
    delaunayincremental.cpp \
    delaunaybase.cpp

HEADERS  += \
    ensure_exception.h \
    mainwindow.h \
    dviewer.h \
    delaunay.h \
    randompointsdialog.h \
    def.h \
    delaunayincremental.h \
    delaunaybase.h

FORMS    += \
    mainwindow.ui \
    randompointsdialog.ui

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/Libs/ -lOpenMeshCore
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/Libs/ -lOpenMeshCored

win32:CONFIG(release, debug|release): PRE_TARGETDEPS += $$PWD/Libs/OpenMeshCore.lib
else:win32:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$PWD/Libs/OpenMeshCored.lib

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/Libs/ -lgtest
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/Libs/ -lgtestd

win32:CONFIG(release, debug|release): PRE_TARGETDEPS += $$PWD/Libs/gtest.lib
else:win32:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$PWD/Libs/gtestd.lib

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/Libs/ -lQGLViewer2
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/Libs/ -lQGLViewerd2

win32: LIBS += -L$$PWD/Libs/ -lfreeglut

INCLUDEPATH += $$PWD/Libs
DEPENDPATH += $$PWD/Libs

DEFINES += _USE_MATH_DEFINES
DEFINES += _VARIADIC_MAX=10

OTHER_FILES += \
    Docs/wps.txt \
    Docs/project.txt \
    Docs/todo.txt

RESOURCES += \
    resource.qrc


