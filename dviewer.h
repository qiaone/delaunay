#ifndef VIEWER_H
#define VIEWER_H

#include <QGLViewer/qglviewer.h>
#include <vector>
#include <memory>
#include <QObject>
#include "delaunay.h"


class DViewer : public QGLViewer
{
    Q_OBJECT

public:
    PointVec points;
    DViewer(QWidget *parent);
    DViewer(Delaunay* delaunay, int mainwindow_width, int mainwindow_height);
    void setParam(Delaunay* delaunay, int mainwindow_width, int mainwindow_height);

public slots:
    void slotBeforeFlip(HHandle hh, VHandle vh, VHandle vh_oppo);
    void slotAfterFlip();

protected :
    virtual void init();
    virtual void draw();
    virtual void keyPressEvent(QKeyEvent *e);
    //virtual void mousePressEvent(QMouseEvent* e);
    void drawMesh();
    void drawMesh(bool m);
    void drawFlip();
    void drawParaboloid(Point lPt, float slice, float stack);

private:
    Delaunay* _delaunay;
    TriMesh _mesh;
    int _mainwindow_width, _mainwindow_height;
	GLuint listName; // display list name
};

#endif // VIEWER_H
