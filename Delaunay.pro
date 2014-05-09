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
    predicates.c

HEADERS  += mainwindow.h \
    triangulation.h \
    halfedge.h \
    vertex.h \
    face.h \
    point2d.h

FORMS    += mainwindow.ui
