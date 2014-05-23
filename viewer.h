#ifndef VIEWER_H
#define VIEWER_H

#include <QGLViewer/qglviewer.h>

namespace DViewer
{

class Viewer : public QGLViewer
{
protected :
    virtual void init();
    virtual void draw();
    virtual QString helpString() const;
    virtual void keyPressEvent(QKeyEvent *e);
    virtual void mousePressEvent(QMouseEvent* e);
    void drawPoints();
};

}
#endif // VIEWER_H
