#ifndef VIEWER_H
#define VIEWER_H

#include <QGLViewer/qglviewer.h>
#include <memory>
#include "Delaunay.h"


class DViewer : public QGLViewer
{
public:
    PointVec points;
    DViewer(QWidget *parent);
    void setParam(std::unique_ptr<Delaunay> delaunay, int mainwindow_width, int mainwindow_height);
    DViewer(std::unique_ptr<Delaunay> delaunay, int mainwindow_width, int mainwindow_height);
protected :
    virtual void init();
    virtual void draw();
    virtual void keyPressEvent(QKeyEvent *e);
    //virtual void mousePressEvent(QMouseEvent* e);
    void drawMesh();
    void drawParaboloid(Point lPt, float slice, float stack);

private:
    std::unique_ptr<Delaunay> _delaunay;
    int _mainwindow_width, _mainwindow_height;
	GLuint listName; // display list name
};

#endif // VIEWER_H
