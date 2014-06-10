#ifndef VIEWER_H
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
    DViewer(DelaunayIncremental* delaunay_inc_, int mainwindow_width_, int mainwindow_height_);
    void setParam(DelaunayIncremental* delaunay_inc_, int mainwindow_width_, int mainwindow_height_);
    void initFlipDemoParams(std::array<Point, 4>& flip);

    void showResult3D();
    void showBeforeFlip3D();
    void showAfterFlip3D();
    void clearAfterFlip3D();
    void toggleShowParaboloid();

protected :
    virtual void init();
    virtual void draw();
    void drawMesh();
    void drawParaboloid(Point bottom_point, float slice, float stack);

private:
    bool isDrawBeforeFlip;
    bool isDrawAfterFlip;
    void drawBeforeFlip();
    void drawAfterFlip();
    bool isShowParaboloid;

    std::array<Point, 4> flip_space_points;
    DelaunayIncremental* delaunay_inc;
    int mainwindow_width, mainwindow_height;
    GLuint paraboloidListId; // display list name
    bool isDrawResult;
};

#endif // VIEWER_H
