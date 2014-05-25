#ifndef VIEWER_H
#define VIEWER_H

#include <QGLViewer/qglviewer.h>
#include <memory>
#include "Delaunay.h"

namespace DViewer
{

class Viewer : public QGLViewer
{
public:
    PointVec points;
    Viewer(std::unique_ptr<Delaunay> delaunay);
protected :
    virtual void init();
    virtual void draw();
    //virtual QString helpString() const;
    virtual void keyPressEvent(QKeyEvent *e);
    //virtual void mousePressEvent(QMouseEvent* e);
    void drawPoints();
private:
    std::unique_ptr<Delaunay> mdelaunay;
};

}
#endif // VIEWER_H
