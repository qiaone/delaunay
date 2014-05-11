#-------------------------------------------------
#
# Project created by QtCreator 2014-05-09T11:39:58
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Delaunay
TEMPLATE = app

SOURCES += main.cpp\
        mainwindow.cpp \
    triangulation.cpp \
    halfedge.cpp \
    vertex.cpp \
    face.cpp \
    point2d.cpp \
    predicates.c \
    ensureexception.cpp \
    Tests/ensureexception_test.cpp

HEADERS  += mainwindow.h \
    triangulation.h \
    halfedge.h \
    vertex.h \
    face.h \
    point2d.h \
    ensureexception.h

FORMS    += mainwindow.ui

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/Libs/ -lOpenMeshCore
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/Libs/ -lOpenMeshCored

INCLUDEPATH += $$PWD/Libs
DEPENDPATH += $$PWD/Libs

DEFINES += _USE_MATH_DEFINES
