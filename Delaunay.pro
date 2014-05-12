#-------------------------------------------------
#
# Project created by QtCreator 2014-05-09T11:39:58
#
#-------------------------------------------------

QT       += core gui testlib

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Delaunay
TEMPLATE = app

SOURCES += main.cpp\
    predicates.c \
    triangulation.cpp \
    mainwindow.cpp \
    ensure_exception.cpp \
    Halfedge/face.cpp \
    Halfedge/halfedge.cpp \
    Halfedge/point2d.cpp \
    Halfedge/vertex.cpp \
    Tests/OpenMeshTest.cpp \
    Tests/DemoTest.cpp

HEADERS  += \
    ensure_exception.h \
    mainwindow.h \
    triangulation.h \
    Halfedge/face.h \
    Halfedge/halfedge.h \
    Halfedge/point2d.h \
    Halfedge/vertex.h

FORMS    += \
    mainwindow.ui

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/Libs/ -lOpenMeshCore
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/Libs/ -lOpenMeshCored

INCLUDEPATH += $$PWD/Libs
DEPENDPATH += $$PWD/Libs

DEFINES += _USE_MATH_DEFINES

OTHER_FILES += \
    Docs/wps.txt \
    Docs/project.txt \
    Docs/todo.txt
