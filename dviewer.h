﻿#ifndef VIEWER_H
#define VIEWER_H

#include <QGLViewer/qglviewer.h>
#include <vector>
#include <memory>
#include <QObject>
#include "delaunayincremental.h"


class DViewer : public QGLViewer
{
    Q_OBJECT

public:
    PointVec points;
    DViewer(QWidget *parent);
    DViewer(DelaunayBase* delaunay_, int mainwindow_width, int mainwindow_height);
    void setParam(DelaunayBase* delaunay_, int mainwindow_width, int mainwindow_height);
    void initFlipDemoParams(std::array<Point, 4>& flip);

    void showMesh3D();
    void clearMesh3D();
    void showMesh2D();
    void showBeforeFlip3D();
    void showAfterFlip3D();
    void clearAfterFlip3D();
    void toggleShowParaboloid();

    bool isKillMesh2D;

protected :
    virtual void init();
    virtual void draw();
    void drawMesh2D();
    void drawMesh3D();
    void drawParaboloid(Point bottom_point, float slice, float stack, FunType funtype);
    //void resizeGL(int width, int height);

private:
    bool isDrawBeforeFlip;
    bool isDrawAfterFlip;
    void drawBeforeFlip();
    void drawAfterFlip();
    bool isDrawParaboloid;
    bool isDrawMesh3D;
    bool isDrawMesh2D;

    int half_window_width;
    int half_window_height;

    std::array<Point, 4> flip_space_points;
    DelaunayBase* delaunay;
    int mainwindow_width, mainwindow_height;
    GLuint paraboloidListId[3]; // display list name
};

#endif // VIEWER_H
