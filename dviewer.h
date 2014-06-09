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

protected :
    virtual void init();
    virtual void draw();
    virtual void keyPressEvent(QKeyEvent *e);
    //virtual void mousePressEvent(QMouseEvent* e);
    void drawMesh();
    void drawMesh(bool m);
    void showFlips3D();
    void drawParaboloid(Point lPt, float slice, float stack);

private:
    DelaunayIncremental* delaunay_inc;
    TriMesh _mesh;
    int mainwindow_width, mainwindow_height;
	GLuint listName; // display list name
};

#endif // VIEWER_H
